/** Definitions for serial port **/

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define NB_TICK 390 //20ms -> 312.5 : ~25ms

void init_serial(int speed);
void send_serial(char c);
char get_serial(void);
void init_printf(void);
void init_timer(void);

#endif
