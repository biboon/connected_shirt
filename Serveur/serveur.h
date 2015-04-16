#ifndef __SERVEUR_H__
#define __SERVEUR_H__

typedef struct udpParameters
{
    int size;
    unsigned char packet[1];
} UDPParameters;
    

void traitementUDP(void* arg);
int threadedTraitementUDP(unsigned char* packet, int size);
void traitementTCP (void* arg);
int threadedTraitementTCP (int sock);

#endif
