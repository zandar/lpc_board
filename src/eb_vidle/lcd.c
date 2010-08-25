#include "lcd.h"
#include <lpc21xx.h>
#include <deb_led.h>

void lcd_wait(){
      static int i,j;   //more than enough
      
      i = 2000000;
      j = 1000000;

      deb_led_on(LEDR);
      
      //busy loop
      for(;i > 1;i--)
      {
	for(;j > 1;j--);
	j = 1000000;
      }
      
      deb_led_off(LEDR);
}

void lcd_out_data4(unsigned char val){
     IOCLR0 |= (LCD_DATA);
     IOSET0 |= (val<<4);
}

void lcd_write_nibbles(unsigned char val){

     //higher-order byte
     lcd_wait();
     lcd_en_set();
     lcd_wait();
     lcd_out_data4((val>>4)&0x0F);
     lcd_wait();
     lcd_en_clr();
     lcd_wait();

     //lower-order byte
     lcd_en_set();
     lcd_wait();
     lcd_out_data4((val)&0x0F);
     lcd_wait();
     lcd_en_clr();
     lcd_wait();
}

void lcd_write_control(unsigned char val){
     lcd_rs_clr();
     lcd_write_nibbles(val);
}

void lcd_init(){

	SET_PIN(PINSEL0, LCD_D4, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_D5, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_D6, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_D7, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_EN, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_RS, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, LCD_RW, PINSEL_0);		// inicializace start pinu
	
	IO0DIR &= ~((1<<LCD_D4) | (1<<LCD_D5) | (1<<LCD_D6) | (1<<LCD_D7));
	IO0DIR &= ~((1<<LCD_EN) | (1<<LCD_RS) | (1<<LCD_RW));
        
	//PINSEL0 &= (~LCD_GPIO_SEL0);
        //PINSEL1 &= (~LCD_GPIO_SEL1);
        /* we only work on OUTPUT so far */
	
        //IO0DIR |= LCD_IOALL;
        
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
       lcd_wait();
       lcd_out_data4(0x02);
       lcd_wait();
       lcd_en_clr();
       lcd_wait();

       lcd_write_nibbles(0x28);
       
       //LCD ON
       lcd_write_nibbles(0x0E);

       //Clear Display
       lcd_write_nibbles(0x01);

       //Entry mode
       lcd_write_nibbles(0x06);

}

void lcd_putchar(unsigned char c){

       lcd_rs_set();
       lcd_write_nibbles(c);
}

void lcd_print(unsigned char* str){
      int i;


      //limit 1 line display for prints
        for (i=0;i<16 && str[i]!=0;i++){
                lcd_putchar(str[i]);
        }

}