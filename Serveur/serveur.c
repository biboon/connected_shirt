/** Fichier serveur.c **/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "libcom.h"

int traitement(unsigned char *packet, int nb) {
    printf("%x", packet);
}

int main(int argc,char *argv[]) {
    /* Analyzing options */
    if (argc != 2){
        fprintf(stderr, "Syntax: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Server initialization */
    char * port = argv[1]; // Port used to connect
    #ifdef DEBUG
        fprintf(stdout, "Port: %s\n", port);
    #endif
    
    /* Socket description */
    /* TODO */
    int sockUDP = initialisationSocketUDP(port);
    if (sockUDP < 0) { perror("serveur.initialisationSocketUDP"); exit(EXIT_FAILURE); }
    boucleServeurUDP(sockUDP, &traitement);
    
    return 0;
}
