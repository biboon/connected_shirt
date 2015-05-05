#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "ethernet.h"
#include "broadcast.h"


unsigned char led_mode = 0, led_freq = 10; // 0: static, 1: blinking


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
		diff = 0;
		for (i = 0; i < 3; i++) { /* Calculates squared distance of acceleration difference */
			ad_init(i);
			input[i] = ad_sample();
			tmp = (input[i] < old_input[i]) ? (old_input[i] - input[i]) : (input[i] - old_input[i]);
			diff += ((int)tmp * (int)tmp);
		}

		angle = my_sqrt(((int)input[0] * (int)input[0] + (int)input[2] * (int)input[2])) / (int)input[1];

		if (samples > 40) { /* Sending data every X seconds */
			samples = 0;
			ad_init(3);
			input[3] = ad_sample();
			build_packet(input, packet);
			slip_send_packet(packet, 29 + DATA_LENGTH);
		} else if (diff > 500 && angle > 1) {
			for (i = 0; i < 4; i++) input[i] = 0xFF;
			led_mode = 1; // blinks the led
			build_packet(input, packet);
			slip_send_packet(packet, 29 + DATA_LENGTH);
		}

		for (i = 0; i < 3; i++) old_input[i] = input[i]; /* Saving old data */
		samples++;
		PORTD ^= 0x80;
		_delay_ms(40);
		PORTD ^= 0x80;
	}
}

void task_get_packet(void) {
	unsigned char packet[29 + DATA_LENGTH], new_id, old_id, freq;

	while (1) {
		slip_recv_packet(packet, 29 + DATA_LENGTH);
		old_id = (packet[28] & 0xF0) >> 4;
		new_id = (packet[29] & 0x0F);
		freq = packet[32];
		if (old_id == get_id()) {
			if (freq == 0) set_id(new_id);
			else led_freq = freq;
		}
		PORTD ^= 0x40;
		_delay_ms(40);
		PORTD ^= 0x40;
	}
}

void task_blink_led(void) {
	while (1) {
		if (led_mode == 0) PORTD = 0x20;
		else if (led_mode == 1) PORTD ^= 0x20;
		_delay_cs(led_freq);
	}
}

void init_led(void) {
	/* We use port D pins 5-6-7 aka connections 5-6-7 */
	DDRD = 0xE0;
	PORTD = 0xE0;
}

void _delay_cs(int t) {
	int i;
	for (i = 0; i < t; i++) _delay_ms(10);
}
