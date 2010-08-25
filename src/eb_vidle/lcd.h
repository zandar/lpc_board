// Copyright (c) 2004 senz at arm.dreamislife.com
//
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

/***************************************/
/* LCD routines for OLIMEX LPC-MT-2106 */
/* 16x2 Character LCD 1602K            */
/* 4-bit mode operation                */
/***************************************/


/* DEFINE IOMASK */
#define  LCD_D4  4	//P0.04
#define  LCD_D5  5	//P0.05
#define  LCD_D6  6	//P0.06
#define  LCD_D7  7	//P0.07
#define  LCD_EN  10	//P0.10
#define  LCD_RS  11	//P0.11
#define  LCD_RW  12	//P0.12

#define  LCD_DATA           ((1<<LCD_D4) | (1<<LCD_D5) | (1<<LCD_D6) | (1<<LCD_D7))
//#define  LCD_IOALL          (LCD_D4|LCD_D5|LCD_D6|LCD_D7|LCD_EN|LCD_RS|LCD_RW)

//#define  LCD_GPIO_SEL0      0x03F0FF00   //MASK for P0.04-P0.07
//#define  LCD_GPIO_SEL1      0x0003F000   //MASK for P0.22-P0.24


/* Functions Header */
/* internal I/O functions */
#define lcd_rs_set() IOSET0 |= (1<<LCD_RS)
#define lcd_rs_clr() IOCLR0 |= (1<<LCD_RS)
#define lcd_en_set() IOSET0 |= (1<<LCD_EN)
#define lcd_en_clr() IOCLR0 |= (1<<LCD_EN)
#define lcd_rw_set() IOSET0 |= (1<<LCD_RW)
#define lcd_rw_clr() IOCLR0 |= (1<<LCD_RW)

/* wait until lcd controller is free */
void lcd_wait();
void lcd_out_data4(unsigned char);
void lcd_write_nibbles(unsigned char);
void lcd_write_control(unsigned char);

/* initialize both the GPIO of lpc and LCD */
void lcd_init();

#define lcd_clear()         lcd_write_control(0x01)
#define lcd_cursor_home()   lcd_write_control(0x02)

#define lcd_display_on()    lcd_write_control(0x0E)
#define lcd_display_off()   lcd_write_control(0x08)

#define lcd_cursor_blink()  lcd_write_control(0x0F)
#define lcd_cursor_on()     lcd_write_control(0x0E)
#define lcd_cursor_off()    lcd_write_control(0x0C)

#define lcd_cursor_left()   lcd_write_control(0x10)
#define lcd_cursor_right()  lcd_write_control(0x14)
#define lcd_display_sleft() lcd_write_control(0x18)
#define lcd_display_sright() lcd_write_control(0x1C)



/* put a character out to lcd */
void lcd_putchar(unsigned char);
/* print a string */
void lcd_print(unsigned char*);











