// Copyright (c) 2004 senz at arm.dreamislife.com
//
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

/***************************************/
/* LCD routines for OLIMEX LPC-2119 */
/* 16x2 Character LCD 1602K            */
/* 4-bit mode operation                */
/***************************************/

#define LCD_START_LINE1  0x00		/* DDRAM address of first char of line 1 */
#define LCD_START_LINE2  0x40		/* DDRAM address of first char of line 2 */
#define LCD_DDRAM  	 7		/* DB7: set DD RAM address */
#define LCD_WAIT	 1500		/* Time delay betwen signals in microseconds us */

/* DEFINE IOMASK */
#define  LCD_D4  0x10	//P0.04
#define  LCD_D5  0x20	//P0.05
#define  LCD_D6  0x40	//P0.06
#define  LCD_D7  0x80	//P0.07
#define  LCD_EN  0x400	//P0.10
#define  LCD_RS  0x800	//P0.11
#define  LCD_RW  0x1000	//P0.12

#define  LCD_DATA           ((LCD_D4) | (LCD_D5) | (LCD_D6) | (LCD_D7))
#define  LCD_IOALL          (LCD_D4|LCD_D5|LCD_D6|LCD_D7|LCD_EN|LCD_RS|LCD_RW)

#define  LCD_GPIO_SEL0      0x03F0FF00   //MASK for P0.04-P0.07, P0.10-P0.12

/* Functions Header */
/* internal I/O functions */
#define lcd_rs_set() IOSET0 |= (LCD_RS)
#define lcd_rs_clr() IOCLR0 |= (LCD_RS)
#define lcd_en_set() IOSET0 |= (LCD_EN)
#define lcd_en_clr() IOCLR0 |= (LCD_EN)
#define lcd_rw_set() IOSET0 |= (LCD_RW)
#define lcd_rw_clr() IOCLR0 |= (LCD_RW)

/* wait until lcd controller is free */
void lcd_wait();
void lcd_out_data4(unsigned char);
void lcd_write_nibbles(unsigned char);
void lcd_write_control(unsigned char);

/* set cursor to specified position */
void lcd_gotoxy(unsigned char, unsigned char);

/* initialize both the GPIO of lpc and LCD */
void lcd_init();

#define lcd_clear()         lcd_write_control(0x01)
#define lcd_cursor_home()   lcd_write_control(0x02)

#define lcd_display_on()    lcd_write_control(0x0C)
#define lcd_display_off()   lcd_write_control(0x08)

#define lcd_cursor_blink()  lcd_write_control(0x0F)
#define lcd_cursor_on()     lcd_write_control(0x0E)
#define lcd_cursor_off()    lcd_write_control(0x0C)

#define lcd_cursor_left()   lcd_write_control(0x10)
#define lcd_cursor_right()  lcd_write_control(0x14)
#define lcd_display_sleft() lcd_write_control(0x18)
#define lcd_display_sright() lcd_write_control(0x1C)



/* put a character out to lcd */
void lcd_putchar(char);
/* print a string */
void lcd_print(char*);











