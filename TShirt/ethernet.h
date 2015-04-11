/** Definitions pour le forgeage de paquet **/

#ifndef __PACKET_H__
#define __PACKET_H__

#define TEAM_ID 7
#define DATA_LENGTH 4
#define UDP_SRC_PORT    12345
#define UDP_DEST_PORT   12345

void build_packet (unsigned char* data, unsigned char* packet);
extern const unsigned char skel_packet[33];

#endif
