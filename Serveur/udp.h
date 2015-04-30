#ifndef __UDP_H__
#define __UDP_H__

void threadedTraitementUDP(void* arg);
void traitementUDP(unsigned char* packet, int size);
void startUDPServer(void* arg);

#endif
