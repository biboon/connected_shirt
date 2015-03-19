/** Definitions for serial port **/

#ifndef __SERIAL_H__
#define __SERIAL_H__

void init_serial(int speed);
void send_serial(unsigned char c);
unsigned char get_serial(void);
void init_printf(void);
int send_serial_printf(unsigned char c, FILE *stream);

#endif
