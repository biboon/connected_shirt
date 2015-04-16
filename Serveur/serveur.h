#ifndef __SERVEUR_H__
#define __SERVEUR_H__

typedef struct udpParameters
{
    int size;
    unsigned char packet[1];
} UDPParameters;
    

void traitementUDP(void* arg);
void threadedTraitementUDP(unsigned char* packet, int size);
void traitementTCP (void* arg);
void threadedTraitementTCP (int sock);

#endif
