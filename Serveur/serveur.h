#ifndef __SERVEUR_H__
#define __SERVEUR_H__

/** Structures definitions **/
typedef struct udpParameters {
    int size;
    unsigned char packet[1];
} UDPParameters;
    
/** Functions **/
void hand(int sig);

#endif
