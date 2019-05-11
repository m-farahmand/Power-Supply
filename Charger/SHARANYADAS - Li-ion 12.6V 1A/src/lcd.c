#include "lcd.h"


void lcd_command(unsigned char command)
{
   LCD_PRT = (LCD_PRT & 0x0F) | (command & 0xF0);
   LCD_PRT &= ~ (1 << LCD_RS);
  LCD_PRT &= ~ (1 << LCD_RW); 
   LCD_PRT |= (1 << LCD_EN);
   _delay_us(1);
   LCD_PRT &= ~ (1 << LCD_EN);
   _delay_us(20);
   LCD_PRT = (LCD_PRT & 0x0F) | (command << 4);
   LCD_PRT |= (1 << LCD_EN);
   _delay_us(1);
   LCD_PRT &= ~(1 << LCD_EN);
   }
   
   void lcd_data(unsigned char data)
   {
      LCD_PRT = (LCD_PRT & 0x0F) | (data & 0xF0);
      LCD_PRT |= (1 << LCD_RS);
      LCD_PRT &= ~(1 << LCD_RW);
      LCD_PRT |= (1 << LCD_EN);
      _delay_us(1);
      LCD_PRT &= ~(1 << LCD_EN);
      
      
      LCD_PRT = (LCD_PRT & 0x0F) | (data << 4);
      LCD_PRT |= (1 << LCD_RS);
      LCD_PRT &= ~(1 << LCD_RW);
      LCD_PRT |= (1 << LCD_EN);
      _delay_us(1);
      LCD_PRT &= ~(1 << LCD_EN);         
      _delay_us(100);
	  }
   
   void lcd_init(void)
      {
	 LCD_DDR = 0xFF;
	 LCD_PRT &= ~(1 << LCD_EN);
	 _delay_us(2000);
	 lcd_command(0x33);
	 _delay_us(100);
	 lcd_command(0x32);
	 _delay_us(100);
	 lcd_command(0x28);
	 _delay_us(100);
	 lcd_command(0x0c);
	 _delay_us(100);
	 lcd_command(0x01);
	 _delay_us(2000);
	 lcd_command(0x06);
	 _delay_us(100);
      }
      
void lcd_gotoxy(unsigned char x, unsigned char y)
{
unsigned char FIRSTCHARADDR[] = {0x80,0xc0,0x94,0xd4};// four line's address for 20x4 lcd
lcd_command(FIRSTCHARADDR[y-1]+x-1);
_delay_us(100);
}

void lcd_print(char*str)
{
   unsigned char i=0;
   while(str[i] != 0)
   { 
   lcd_data(str[i]);
   i++;
   }
}
