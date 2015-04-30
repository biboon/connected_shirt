#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "udp.h"

#include "http.h"
#include "teams.h"
#include <libcom.h>
#include <libthrd.h>


void saveData(unsigned char* packet, int size) {
    if (size == 5) {
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
        sprintf(filename, "./www/binaries/team_%d.bin", team);
        P(FILE_MUTEX + team);
        FILE* out = fopen(filename, "ab");
        fwrite(data, sizeof(Message), 1, out);
        fclose(out);
        V(FILE_MUTEX + team);
        free(data);
        /* Finished */
    } else {
        fprintf(stderr, "Received packet with wrong size\n");
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
