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
    int i = 0;
    unsigned short int cksm = 0, to_add = 0, diff = 0;
    
    /* Building base packet with constants and data*/
    for (i = 0; i < 29 + DATA_LENGTH; i++) packet[i] = skel_packet[i];
    for (i = 0; i < DATA_LENGTH; i++) packet[i + 29] = data[i];
    
    /* Calculating IP checksum */
    for (i = 0; i < 20; i += 2) {
        if (i != 10) {
            to_add = ((packet[i] << 8) & 0xFF00) + (packet[i + 1] & 0xFF);
            diff = 0xFFFF - cksm;
            if (to_add > diff) cksm = to_add - diff;
            else cksm += to_add;
        }
    }
    cksm = ~cksm; /* Final one's complement */
    /* Setting IP checksum in packet */
    packet[10] = (cksm >> 4) & 0xFF;
    packet[11] = cksm & 0xFF;
    
    /* Calculating UDP checksum */
    for (i = 20; i < 33; i += 2) { /* UDP header */
        if (i != 26) {
            to_add = ((packet[i] << 8) & 0xFF00);
            if (i != 32) to_add += (packet[i + 1] & 0xFF);
            diff = 0xFFFF - cksm;
            if (to_add > diff) cksm = to_add - diff;
            else cksm += to_add;
        }
    }
    for (i = 12; i < 20; i += 2) { /* Src and dest addresses */
        to_add = ((packet[i] << 8) & 0xFF00) + (packet[i + 1] & 0xFF);
        diff = 0xFFFF - cksm;
        if (to_add > diff) cksm = to_add - diff;
        else cksm += to_add;
    }
    to_add = packet[9] + packet [25]; /* Protocol + UDP length */
    diff = 0xFFFF - cksm;
    if (to_add > diff) cksm = to_add - diff;
    else cksm += to_add;
    cksm = ~cksm; /* Final one's complement */
    /* Setting UDP checksum in packet */
    packet[26] = (cksm >> 4) & 0xFF;
    packet[27] = cksm & 0xFF;
}
