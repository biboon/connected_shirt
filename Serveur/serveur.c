/** Fichier serveur.c **/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "libcom.h"

int traitementMsg(unsigned char *packet, int nb) {
    #ifdef DEBUG
        fprintf(stderr, "New packet succesfully received! size: %d\n", nb);
    #endif
    //TODO
    return 0;
}

int traitementTCP(int fd) {
    #ifdef DEBUG
        fprintf(stderr, "Started new TCP client thread on fd: %d\n", fd);
    #endif
    //TODO
    return 0;
}

int main(int argc,char *argv[]) {
    /* Analyzing options */
    int option = 0;
    char* port = "4000";
    while((option = getopt(argc, argv, "p:")) != -1) {
        if (option == 'p') port = optarg;
        else fprintf(stderr, "Unrecognized option, using default port\n");
    }
    
    /* Starting UDP messages server */
    serveurMessages(port, &traitementMsg);
    
    /* Starting TCP server */
    int sockTCP = initialisationServeur("4200", MAX_CONNEXIONS);
    boucleServeur(sockTCP, &traitementTCP);
    
    
    return 0;
}
