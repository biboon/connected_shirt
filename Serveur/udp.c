#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libthrd.h>
#include <libcom.h>
#include "udp.h"
#include "serveur.h"
#include "http.h"

void threadedTraitementUDP(void* arg) {
    UDPParameters* param = arg;
    #ifdef DEBUG
        fprintf(stderr, "Started new UDP process thread, packet size: %d\n", param->size);
    #endif
    fillDataTab(param->size, param->packet);
}

void traitementUDP(unsigned char* packet, int size) {
    int allocated = sizeof(UDPParameters) + size - 1;
    UDPParameters *param = (UDPParameters*) malloc(allocated);
    memcpy(param->packet, packet, size);
    param->size = size;
    printf("%d", param->size);
    if (lanceThread(&threadedTraitementUDP, (void *) param, allocated) < 0) {
        perror("traitementUDP.lanceThread"); exit(EXIT_FAILURE);
    }
    free(param);
}

void startUDPServer(void* arg) {
    serveurMessages((char*)arg, &traitementUDP);
}
