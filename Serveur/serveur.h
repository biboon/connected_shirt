#ifndef __SERVEUR_H__
#define __SERVEUR_H__


/** Structures **/
typedef struct udpParameters {
    int size;
    unsigned char packet[1];
} UDPParameters;
    

/** Functions **/
void threadedTraitementUDP(void* arg);
void traitementUDP(unsigned char* packet, int size); /* Starts traitementUDP in a new thread */
void threadedTraitementTCP (void* arg);
void traitementTCP (int sock); /* Starts traitementTCP in a new thread */

#endif
