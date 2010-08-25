#include <uart.h>




/**
 *	Send one char to uart.
 */
void uart_send_char(char val)
{
	uart0SendCh(val);
}

/**
 *	Read one char from uart.
 */
char uart_get_char(void)
{
	return uart0GetCh();
}

/**
 *	Initialize UART - platform dependent
 */
void init_uart(void)
{
	init_uart0((int)9600, UART_BITS_8, UART_STOP_BIT_1, UART_PARIT_OFF, 0 );
}


/**
 *	Send string to serial output in ASCII code. .
 *	@param data[]	string to print
 */
void send_rs_str(const char data[])
{
	
	int i = 0;
	int j = 0;
	
	for (j = 0; j < 255; j++)
	{
		if(data[j] == 0) break;
	}
		
	for (i= 0 ; i < j; i++)
	{
		uart_send_char(data[i]);
	}
}

/**
 *	Send int value to serial output in ASCII code. Removes unused zeros.
 *	@param val	value to print
 */
void send_rs_int(int val)
{
	char dat[8];
	int i;
	int pom = 0;
	
	for(i = 0; i < 8; i++)
	{
		dat[i] = (val & 0xF) + 0x30;
		if(dat[i] > '9')
			dat[i] += 7;
		val >>= 4;
	}
	
	for(i = 0; i < 8; i++)
	{
		if((pom == 0) & (dat[7-i] == '0')) 
		{
			if((i == 6) | (i == 7))
				uart_send_char('0');		
			continue;
		}
		pom = 1;
		uart_send_char(dat[7-i]);
	}
	
}
