/** Definitions for serial port **/

#ifndef __SERIAL_H__
#define __SERIAL_H__

void init_serial(int speed);
void send_serial(char c);
char get_serial(void);
void init_printf(void);

#endif
