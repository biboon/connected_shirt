/** Fichier serveur.c **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "libcom.h"
#include "libthrd.h"
#include "serveur.h"


void threadedTraitementUDP(void* arg) {
    UDPParameters* param = arg;
    #ifdef DEBUG
        fprintf(stderr, "Started new UDP process thread, packet size: %d\n", param->size);
    #endif
    sleep(10);
    #ifdef DEBUG
        fprintf(stderr, "Finished processing UDP packet\n");
    #endif
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

void threadedTraitementTCP (void* arg) {
    int sock = *((int *)arg);
    #ifdef DEBUG
        fprintf(stderr, "Started new TCP process thread on sock #%d\n", sock);
    #endif
    FILE* fd = fdopen(sock, "r");
    if (fd == NULL) { perror("threadedTraitementTCP.fdopen"); exit(EXIT_FAILURE); }
    fprintf(fd, "Hello World!\n");
    sleep(10);
    if (fclose(fd) < 0) { perror("threadedTraitementTCP.fclose"); exit(EXIT_FAILURE); }
    #ifdef DEBUG
        fprintf(stderr, "Closed sock #%d\n", sock);
    #endif
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

void startUDPServer(void* arg) {
    serveurMessages((char*)arg, &traitementUDP);
}

int main(int argc,char *argv[]) {
    /* Analyzing options */
    int option = 0;
    char* portUDP = "12345";
    char* portTCP = "4200";
    
    while((option = getopt(argc, argv, "p:")) != -1) {
        if (option == 'p') portUDP = optarg;
        else fprintf(stderr, "Unrecognized option, using default UDP port\n");
    }
    
    /* Starting UDP messages server */
    lanceThread(startUDPServer, (void*) portUDP, sizeof(portUDP));
    
    /* Starting TCP server */
    lanceThread(startTCPServer, (void*) portTCP, sizeof(portTCP));
    
    while(getLivingThreads() != 0) sleep(1);
    
    return 0;
}
