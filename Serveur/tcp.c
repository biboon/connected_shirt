#include <stdio.h>
#include <stdlib.h>

#include <libthrd.h>
#include <libcom.h>
#include "tcp.h"
#include "http.h"

void threadedTraitementTCP (void* arg) {
	int sock = *((int *)arg);
	#ifdef DEBUG
		fprintf(stderr, "Started new TCP process thread on sock #%d\n", sock);
	#endif
	createHttpClient(sock);
}

void traitementTCP (int sock) {
	int tmp = sock;
	if (lanceThread(&threadedTraitementTCP, (void *) &tmp, sizeof(int)) < 0) {
		perror("traitementTCP.lanceThread"); exit(EXIT_FAILURE);
	}
}

void startTCPServer(void* arg) {
	serveurTCP((char*)arg, &traitementTCP);
}
