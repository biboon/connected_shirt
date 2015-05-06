#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "udp.h"

#include "http.h"
#include "teams.h"
#include <libcom.h>
#include <libthrd.h>


bool check_parity(unsigned char* packet) {
	int i = 1, j;
	bool res = true;
	unsigned char data, b = 1, tmp;
	do {
		data = packet[i];
		tmp = 1;
		for (j = 0; j < 8; j++) if (data & (b << j)) tmp++;
		tmp = tmp % 2;
		res &= ((packet[0] >> (4 - i)) & 0x01) == tmp;
		i++;
	} while (res && i < 5);
	return res;
}


void saveData(unsigned char* packet, int size) {
	if (size == 5 && check_parity(packet)) {
		int team = (int) ((packet[0] & 0xF0) >> 4);
		Message* data = (Message*) malloc(sizeof(Message));
		/* Putting data in structure */
		data->i = team;
		data->x = packet[1];
		data->y = packet[2];
		data->z = packet[3];
		data->t = packet[4];
		data->ts = (long int) time(NULL);

		/* Saving data in binary file */
		char filename[30];
		sprintf(filename, "./www/logs/team_%d.bin", team);
		P(FILE_MUTEX + team);
		FILE* out = fopen(filename, "ab");
		fwrite(data, sizeof(Message), 1, out);
		fclose(out);
		V(FILE_MUTEX + team);
		free(data);
		/* Finished */
	} else {
		fprintf(stderr, "Received invalid packet (wrong size/parity)\n");
	}
	#ifdef DEBUG
		fprintf(stderr, "Finished processing UDP packet\n");
	#endif
}


void threadedTraitementUDP(void* arg) {
	UDPParameters* param = arg;
	#ifdef DEBUG
		fprintf(stderr, "Started new UDP process thread, packet size: %d\n", param->size);
	#endif
	saveData(param->packet, param->size);
}

void traitementUDP(unsigned char* packet, int size) {
	int allocated = sizeof(UDPParameters) + size - 1;
	UDPParameters* param = (UDPParameters*) malloc(allocated);
	memcpy(param->packet, packet, size);
	param->size = size;
	if (lanceThread(&threadedTraitementUDP, (void *) param, allocated) < 0) {
		perror("traitementUDP.lanceThread"); exit(EXIT_FAILURE);
	}
	free(param);
}

void startUDPServer(void* arg) {
	serveurMessages((char*)arg, &traitementUDP);
}
