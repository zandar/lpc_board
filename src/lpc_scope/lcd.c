#include <lpc21xx.h>                            /* LPC21xx definitions */
#include <system_def.h>	
#include <deb_led.h>
#include "def.h"
#include "lcd.h"

void lcd_wait()
{
      
      static uint32_t wait_time = 0;
      
      wait_time = timer_usec;
      
      while(timer_usec <= wait_time + LCD_WAIT);
}

void lcd_out_data4(unsigned char val)
{
     IOCLR0 |= (LCD_DATA);
     IOSET0 |= (val<<4);
}

void lcd_write_nibbles(unsigned char val)
{

     //higher-order byte
     lcd_en_set();
     lcd_wait();
     lcd_out_data4((val>>4)&0x0F);
     lcd_en_clr();
     lcd_wait();

     //lower-order byte
     lcd_en_set();
     lcd_out_data4((val)&0x0F);
     lcd_en_clr();
     lcd_wait();
}

void lcd_write_control(unsigned char val)
{
     lcd_rs_clr();
     lcd_write_nibbles(val);
}


/* Set cursor to specified position */

void lcd_gotoxy(unsigned char x, unsigned char y)
{
    if (y==0)
      lcd_write_control((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
      lcd_write_control((1<<LCD_DDRAM)+LCD_START_LINE2+x);
}

void lcd_init(){
  
	PINSEL0 &= (~LCD_GPIO_SEL0);
	
        /* we only work on OUTPUT so far */
        IO0DIR |= LCD_IOALL;
        
        /* IO init complete, init LCD */

       /* init 4-bit ops*/
       lcd_wait();
       lcd_rs_clr();
       lcd_rw_clr();
       lcd_en_clr();

       //wait VDD raise > 4.5V
       lcd_wait();
       
       //dummy inst 
       lcd_write_nibbles(0x30);
       lcd_write_nibbles(0x30);
       lcd_write_nibbles(0x30);

       //FUNCTION SET
       //001DL  N F XX
       //DL=1: 8bit
       //DL=0: 4bit
       //N=0: 1 line display
       //N=1: 2 line display
       //F=0: 5x7 dots
       //F=1: 5x10 dots

       //our case:
       //0010 1000
       lcd_en_set();
       lcd_out_data4(0x02);
       lcd_en_clr();  
       lcd_wait();

       lcd_write_nibbles(0x28);
       
       //LCD ON
       lcd_write_nibbles(0x0C);

       //Clear Display
       lcd_write_nibbles(0x01);

       //Entry mode
       lcd_write_nibbles(0x06);

}

void lcd_putchar(char c){

       lcd_rs_set();
       lcd_write_nibbles(c);
}

void lcd_print(char* str){
      int i;

      //limit 1 line display for prints
        for (i=0;i<16 && str[i]!=0;i++){
                lcd_putchar(str[i]);
        }
}
