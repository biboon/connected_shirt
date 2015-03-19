#ifndef __SKEL_PACKET_H__
#define __SKEL_PACKET_H__

#define TEAM_ID 7
#define DATA_LENGTH 4
#define UDP_SRC_PORT    5000
#define UDP_DEST_PORT   5000

const unsigned char skel_packet[33] = {
    69, 0, 0, 29 + DATA_LENGTH, /* IP */
    0, TEAM_ID, 64, 0,
    10, 17, 0, 0, /* last 2 bytes: IP checksum */
    172, 26, 79, 200 + 2 * TEAM_ID,
    172, 26, 79, 201 + 2 * TEAM_ID,
    UDP_SRC_PORT / 256, UDP_SRC_PORT % 256, UDP_DEST_PORT / 256, UDP_DEST_PORT % 256, /* UDP */
    0, 9 + DATA_LENGTH, 0, 0,
    TEAM_ID * 16, 0, 0, 0, /* Datas */
    0};

#endif
