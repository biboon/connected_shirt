/* Main program going on lilypad */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "ethernet.h"


int sqrt(int n) {
    int res = n / 2, int i;
    for (i = 0; i < 6; i++) res = (res + n / res) / 2;
    return res;
}


int main(void) {
    /* Connection initialization */
    init_printf();
    unsigned char input[DATA_LENGTH], old_input[DATA_LENGTH] = {123,123,123}, tmp;
    unsigned char packet[29 + DATA_LENGTH];
    
    int i, samples = 0, angle, diff;
    while (1) {
        diff = 0;
        for (i = 0; i < 3; i++) { /* Calculates squared distance of acceleration difference */
            ad_init(i);
            input[i] = ad_sample();
            tmp = (input[i] < old_input[i]) ? (old_input[i] - input[i]) : (input[i] - old_input[i]);
            diff += (int)tmp * (int)tmp;
        }
        
        angle = sqrt((int)input[0] * (int)input[0] + (int)input[2] * (int)input[2]) / (int)input[1];
        
        if (samples > 250) {
            samples = 0;
            input[3] = ad_sample();
            build_packet(input, packet);
            slip_send_packet(packet, 29 + DATA_LENGTH);
        } else if (diff > 500 && angle > 2) {
            for (i = 0; i < 4; i++) input[i] = 0xFF;
            build_packet(input, packet);
            slip_send_packet(packet, 29 + DATA_LENGTH);
        }
        
        for (i = 0; i < 3; i++) old_input[i] = input[i]; /* Saving old data */
        samples++; _delay_ms(20); /* getting 50 samples per second */
    }
}
