/** Fichier serveur.c **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "libcom.h"
#include "libthrd.h"

int traitementMsg(unsigned char *packet, int nb) {
    #ifdef DEBUG
        fprintf(stderr, "New packet succesfully received! size: %d\n", nb);
    #endif
    //TODO
    return 0;
}

int traitementTCP(int sock) {
    #ifdef DEBUG
        fprintf(stderr, "Started new TCP listening loop thread on sock: %d\n", sock);
    #endif
    FILE* fd = fdopen(sock, "r");
    if (fd == NULL) { perror("traitementTCP.fdopen"); exit(EXIT_FAILURE); }
    fprintf(fd, "Hello World!\n");
    if (fclose(fd) < 0) { perror("traitementTCP.fclose"); exit(EXIT_FAILURE); }
    #ifdef DEBUG
        fprintf(stderr, "Closed sock: %d\n", sock);
    #endif
    while (1) printf("blblblbl\n");
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
    //serveurMessages(port, &traitementMsg);
    
    /* Starting TCP server */
    #ifdef DEBUG
        fprintf(stderr, "Initializing TCP server on port 4200\n");
    #endif
    int sockTCP = initialisationServeur("4200", MAX_CONNEXIONS);
    #ifdef DEBUG
        fprintf(stderr, "Starting TCP server loop on port 4200 listening sock: %d\n", sockTCP);
    #endif
    boucleServeur(sockTCP, &traitementTCP);
    
    return 0;
}
