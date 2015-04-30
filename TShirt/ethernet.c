#include <avr/io.h>
#include <stdio.h>

#include "ethernet.h"

const unsigned char skel_packet[33] = {
    0x45, 0, 0, 29 + DATA_LENGTH, /* IP */
    0, TEAM_ID, 64, 0,
    10, 17, 0, 0, /* last 2 bytes: IP checksum */
    172, 26, 79, 201 + 2 * TEAM_ID,
    172, 26, 79, 255, /* broadcast mode */
    (UDP_SRC_PORT >> 8) & 0xFF, UDP_SRC_PORT & 0xFF, (UDP_DEST_PORT >> 8) & 0xFF, UDP_DEST_PORT & 0xFF, /* UDP */
    0, 9 + DATA_LENGTH, 0, 0,
    (TEAM_ID << 4) & 0xF0, 0, 0, 0, /* Datas */
    0};


/* builds the UDP/IP packet */
void build_packet (unsigned char* data, unsigned char* packet) {
/* Building base packet with constants and data*/
    unsigned int i;
    for (i = 0; i < 29 + DATA_LENGTH; i++) packet[i] = skel_packet[i];
    for (i = 0; i < DATA_LENGTH; i++) packet[29 + i] = data[i];
    
    do_ip_cksm(packet);
    do_parity(packet);
    do_udp_cksm(packet);
}


void do_parity (unsigned char* packet) {
    unsigned int b = 1, cpt, i, j;
    unsigned char data;
    
    for (j = 0; j < DATA_LENGTH; j++) {
        cpt = 0;
        data = packet[29 + j];
        for (i = 0; i < 8; i++) if (data & (b << i)) cpt++;
        packet[28] = packet[28] | ((unsigned char)(cpt + 1) % 2 << (DATA_LENGTH - j));
    }
}


void do_udp_cksm (unsigned char* packet) {
    int i = 0;
    uint32_t cksm = 0;
    
    /* Calculating UDP checksum */
    cksm = 0;
    for (i = 20; i < 33; i += 2) { /* UDP header */
        if (i != 26) {
            cksm += ((uint32_t)packet[i] & 0xFF)  << 8;
            if (i != 32) cksm += (uint32_t)packet[i + 1] & 0xFF;
        }
    }
    for (i = 12; i < 20; i += 2) { /* Src and dest addresses */
        cksm += (((uint32_t)packet[i] & 0xFF) << 8) + ((uint32_t)packet[i + 1] & 0xFF);
    }
    cksm += ((uint32_t)packet[9] + (uint32_t)packet [25]) & 0xFF; /* Protocol + UDP length */
    while (cksm > 0x0000FFFF) {
        cksm = (cksm & 0x0000FFFF) + ((cksm & 0xFFFF0000) >> 16); 
    }
    cksm = ~cksm; /* Final one's complement */
    packet[26] = (cksm >> 8) & 0xFF; /* Setting UDP checksum in packet */
    packet[27] = cksm & 0xFF;
}


void do_ip_cksm (unsigned char* packet) {
    int i = 0;
    uint32_t cksm = 0;
    
    /* Calculating IP checksum */
    for (i = 0; i < 20; i += 2) {
        if (i != 10) {
            cksm += (((uint32_t)packet[i] & 0xFF) << 8) + ((uint32_t)packet[i + 1] & 0xFF);
        }
        while (cksm > 0xFFFF) {
            cksm = (cksm & 0x0000FFFF) + ((cksm & 0xFFFF0000) >> 16); 
        }
    }
    cksm = ~cksm; /* Final one's complement */
    packet[10] = (cksm & 0xFF00) >> 8; /* Setting IP checksum in packet */
    packet[11] = cksm & 0xFF;
}
