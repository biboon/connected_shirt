#ifndef __UDP_H__
#define __UDP_H__

/** Structures definitions **/
typedef struct udpParameters {
    int size;
    unsigned char packet[1];
} UDPParameters;

bool check_parity(unsigned char* packet);
void threadedTraitementUDP(void* arg);
void traitementUDP(unsigned char* packet, int size);
void startUDPServer(void* arg);
void saveData(unsigned char* packet, int size);

#endif
