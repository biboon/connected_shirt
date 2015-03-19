/* Main program going on lilypad */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "skel_packet.h"
//#include "ethernet.h"
//#include "socket.h"

void build_packet (unsigned char* data, unsigned char* packet) {
    int i = 0;
    for (i = 0; i < 29 + DATA_LENGTH; i++) *(packet + i) = *(skel_packet + i);
    for (i = 0; i < DATA_LENGTH; i++) *(packet + 30 + i) = *(data + i);
    
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
    *(packet + 10) = ip_cksm / 256;
    *(packet + 11) = ip_cksm % 256;
}

int main(void) {
    /* Connection initialization */
    init_printf();
    unsigned char input[DATA_LENGTH] = {'a','b','c','e'};
    unsigned char packet[29 + DATA_LENGTH];

    while (1) {
        build_packet(input, packet);
        slip_send_packet(packet, 29 + DATA_LENGTH);
        _delay_ms(1000);
    }
}
