/** Fichier serveur.c **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <libcom.h>
#include <libthrd.h>
#include "http.h"
#include "tcp.h"
#include "udp.h"

#define TIMEOUT 30

/** Global variables **/
static bool _stop = false;
static struct sigaction action;


void hand(int sig) {
    if (sig == SIGINT) {
        printf("SIGINT signal received, closing servers...\n");
        stopServers();
        _stop = true;
    } else perror("Unrecognized signal received");
}


int main(int argc,char *argv[]) {
    /* Analyzing options */
    int option = 0;
    char* portUDP = "12345";
    char* portTCP = "4200";
    /* Getting options */
    while((option = getopt(argc, argv, "p:")) != -1) {
        if (option == 'p') portUDP = optarg;
        else fprintf(stderr, "Unrecognized option, using default UDP port\n");
    }
    
    /* Signal handling initialization */
    action.sa_handler = hand;
    sigaction(SIGINT, &action, NULL);
    
    /* Starting UDP messages server */
    lanceThread(startUDPServer, (void*) portUDP, sizeof(portUDP));
    /* Starting TCP server */
    lanceThread(startTCPServer, (void*) portTCP, sizeof(portTCP));
    
    /* Main process sleeping while servers are working */
    while (!_stop) sleep(1);
    
    /* Waiting for threads to terminate */
    int timeout = 0;
    while (getLivingThreads() != 0 && timeout < TIMEOUT) { sleep(1); timeout++; }
    if (timeout == TIMEOUT) printf("Servers quit: timeout\n");
    
    return 0;
}
