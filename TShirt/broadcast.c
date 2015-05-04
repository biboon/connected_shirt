#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "ethernet.h"


int my_sqrt(int n) {
	int res = n >> 1, i;
	for (i = 0; i < 6; i++) res = (res + n / res) / 2;
	return res;
}

void task_send_samples(void) {
	unsigned char input[DATA_LENGTH], old_input[DATA_LENGTH] = {123,123,123}, tmp;
	unsigned char packet[29 + DATA_LENGTH];

	int i, samples = 0, angle, diff;
	while (1) {
		cli();
		diff = 0;
		for (i = 0; i < 3; i++) { /* Calculates squared distance of acceleration difference */
			ad_init(i);
			input[i] = ad_sample();
			tmp = (input[i] < old_input[i]) ? (old_input[i] - input[i]) : (input[i] - old_input[i]);
			diff += ((int)tmp * (int)tmp);
		}

		angle = my_sqrt(((int)input[0] * (int)input[0] + (int)input[2] * (int)input[2])) / (int)input[1];

		if (samples > 40) { /* Sending data every 3 second */
			samples = 0;
			ad_init(3);
			input[3] = ad_sample();
			build_packet(input, packet);
			slip_send_packet(packet, 29 + DATA_LENGTH);
		} else if (diff > 500 && angle > 1) {
			for (i = 0; i < 4; i++) input[i] = 0xFF;
			build_packet(input, packet);
			slip_send_packet(packet, 29 + DATA_LENGTH);
		}

		for (i = 0; i < 3; i++) old_input[i] = input[i]; /* Saving old data */
		samples++; _delay_ms(25); /* getting 50 samples per second */
		sei();
	}
}

void task_get_packet(void) {
	while (1){
		//send_serial('P');
		_delay_ms(100);
	}
}

void task_set_led(void) {
	while (1) {
		//send_serial('L');
		_delay_ms(100);
	}
}
