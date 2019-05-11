//This library is intended to use with both 16x2 & 20x4 characted lcds

#ifndef LCD_H
#define LCD_H

#include<avr/io.h>
#include<util/delay.h>

#define LCD_PRT PORTD//define lcd port here for data & command pins
#define LCD_DDR DDRD
#define LCD_PIN PIND
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_init(void);
void lcd_gotoxy(unsigned char, unsigned char);
void lcd_print(char*str);


#endif