/* Main program going on lilypad */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "ethernet.h"

int main(void) {
    /* Connection initialization */
    init_printf();
    unsigned char input[DATA_LENGTH], absValue, tmp;
    unsigned char packet[29 + DATA_LENGTH];
    
    int i, samples = 0;
    while (1) {
        absValue = 0;
        for (i = 0; i < 3; i++) { /* getting x,y,z samples */
            ad_init(i);
            tmp = ad_sample();
            absValue += (tmp < 123) ? (123 - tmp) * (123 - tmp) : (tmp - 123) * (tmp - 123);
            input[i] = tmp;
        }
        
        #if 0
            No gravity means 123 value measure
            ADXL335 : 300 mV/g
            lilypad's adc : 12.9 mV/bit
            1g = 24~25 bits (+123)
        #endif
        
        if (absValue > 1764 || input[2] > 142 || input[2] < 102) { /* fall if acc > 2g or horizontal, 1764 = 42*42 */
            for (i = 0; i < 4; i++) input[i] = 0xFF;
            build_packet(input, packet);
            slip_send_packet(packet, 29 + DATA_LENGTH);
        } else if (samples > 250) { /* sending sample every 5 seconds */
            samples = 0;
            ad_init(3); /* getting temp */
            input[3] = ad_sample();
            build_packet(input, packet);
            slip_send_packet(packet, 29 + DATA_LENGTH);
        }
        
        samples++; _delay_ms(20); /* getting 50 samples per second */
    }
}
