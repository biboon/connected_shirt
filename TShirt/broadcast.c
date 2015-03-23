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
