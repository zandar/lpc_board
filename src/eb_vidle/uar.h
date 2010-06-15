#ifndef _UAR_H
#define _UAR_H


void send_rs_int(int val);
void send_rs_str(const char data[]);
void init_uart(void);
char uart_get_char(void);
void uart_send_char(char val);


#endif

