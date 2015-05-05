#ifndef __BROADCAST_H__
#define __BROADCAST_H__


int my_sqrt(int n); /* Calculates roughly sqrt n */
void task_send_samples(void);
void task_get_packet(void);
void task_blink_led(void);
void init_led(void);
void _delay_cs(int t);


#endif
