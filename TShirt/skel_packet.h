/** Definitions pour le forgeage de paquet **/

#ifndef __SKEL_PACKET_H__
#define __SKEL_PACKET_H__

#define TEAM_ID 7
#define DATA_LENGTH 4
#define UDP_SRC_PORT    12345
#define UDP_DEST_PORT   12345

const unsigned char skel_packet[33] = {
    69, 0, 0, 29 + DATA_LENGTH, /* IP */
    0, TEAM_ID, 64, 0,
    10, 17, 0, 0, /* last 2 bytes: IP checksum */
    172, 26, 79, 200 + 2 * TEAM_ID,
    172, 26, 79, 255, /* broadcast mode */
    UDP_SRC_PORT / 256, UDP_SRC_PORT % 256, UDP_DEST_PORT / 256, UDP_DEST_PORT % 256, /* UDP */
    0, 9 + DATA_LENGTH, 0, 0,
    TEAM_ID * 16, 0, 0, 0, /* Datas */
    0};

void build_packet (unsigned char* data, unsigned char* packet) {
    int i = 0;
    /* Building base packet */
    for (i = 0; i < 29 + DATA_LENGTH; i++) *(packet + i) = *(skel_packet + i);
    for (i = 0; i < DATA_LENGTH; i++) *(packet + 29 + i) = *(data + i);
    
    /* Calculating IP checksum */
    unsigned short int ip_cksm = 0;
    unsigned short int to_add = 0;
    unsigned short int diff = 0;
    for (i = 0; i < 10; i++) {
        if (i != 5) {
            to_add = *(packet + 2 * i) * 256 + *(packet + 2 * i + 1);
            diff = 65535 - ip_cksm;
            if (to_add > diff) ip_cksm = to_add - diff;
            else ip_cksm += to_add;
        }
    }
    ip_cksm = ~ip_cksm;
    
    /* Setting checksum in packet */
    *(packet + 10) = ip_cksm / 256;
    *(packet + 11) = ip_cksm % 256;
}

#endif
