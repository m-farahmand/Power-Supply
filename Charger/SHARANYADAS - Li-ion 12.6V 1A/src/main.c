#include<avr/io.h>
#include<util/delay.h>
#include "lcd.h"
#include"adc.h"

#define ref_volt 5.01
#define div 4
#define ACS714_out (ref_volt/2)



uint16_t volt_temp,amp_temp;
uint8_t OCR_temp,calib_mode,batt,cc_mode,cv_mode,li_ion=1;//to denote li_ion as default battery type



void pwminit(void)
{
TCCR2|=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS20);
DDRB|=(1<<3);
OCR2=0;
}


uint8_t calib_sw_pressed(void)//if calibration switch is pressed or not
{
if(bit_is_clear(PINB,PB0))//check for a key press
{
_delay_ms(25);//debounce time
if(bit_is_clear(PINB,PB0))//check for the real key press
while(bit_is_clear(PINB,PB0));//wait until the key is released
if(calib_mode==0)//if calibartion mode was previously 0
calib_mode=1;//make it 1 to notify that key is pressed to enter calibration mode
else
calib_mode=0;//if no,then make it 0 to exit from calibration mode

return 1;//key press detected,return 1
}
return 0;//false call,return 0
}


void print(uint8_t val)
{
//lcd_data((val/100)+48);
val%=100;
lcd_data((val/10)+48);
val%=10;
lcd_data(val+48);//showing two digits of percentage
}


void print_duty(uint8_t val1)
{
uint16_t temp;

temp=((val1+1)*100)/256;
lcd_gotoxy(14,1);
print((uint8_t)temp);
}


uint16_t measure_volt(void)
{
unsigned int adc_avg;
unsigned int volt;
float temp_val;

adc_avg=read_adc_avg(1,101);
temp_val=(adc_avg * ref_volt * div)/1024;
volt=(unsigned int)(temp_val * 100);
return volt;
}



uint16_t measure_amp(void)
{
float temp_val;
uint16_t result,amp=0;
result=read_adc_avg(0,101);
temp_val=(result * ref_volt)/1024;
if(temp_val>ACS714_out)
{
temp_val=(temp_val-ACS714_out)/0.185;
amp=(uint16_t)(temp_val * 100);
}
return amp;
}



void print_param(uint8_t x1,uint8_t y1,uint16_t val2)
{
lcd_gotoxy(x1,y1);
lcd_data((val2/1000)+48);
val2%=1000;
lcd_data((val2/100)+48);
val2%=100;
lcd_gotoxy((x1+2),y1);
lcd_data('.');
lcd_data((val2/10)+48);//Showing upto 1 digit after decimal point
}


void print_const1(void)
{
lcd_gotoxy(1,1);
lcd_print("Vo-");
lcd_gotoxy(6,1);
lcd_data('.');
lcd_gotoxy(16,1);
lcd_data('%');
lcd_gotoxy(1,2);
lcd_print("Vb-");
lcd_gotoxy(10,2);
lcd_print("Ia-");
lcd_gotoxy(15,2);
lcd_data('.');
}



void calibrate(void)
{
lcd_command(0x01);//clear lcd
_delay_ms(100);
lcd_gotoxy(1,2);
lcd_print("cal mode");
while(calib_mode)//loop and display measured voltag value to calibrate using pot until next button press
{
volt_temp=measure_volt();
print_param(4,1,volt_temp);
_delay_ms(100);
if(calib_sw_pressed())
{
lcd_command(0x01);
_delay_ms(1);
print_const1();
_delay_ms(100);
}
}
}

void charge_li_ion(void)
{

//disable_pwm();
PORTB &=~(1<<1);//off the mosfet in order to read the battery
_delay_ms(10);//delay some time
volt_temp=measure_volt();//measure the battery voltage

//------check if battery is conneted to the charger---------//

if(volt_temp==0)
{
lcd_command(0x01);//clear the LCD
_delay_ms(10);
lcd_gotoxy(1,1);
lcd_print(" Insert battery");
batt=0;

while(batt==0)//This loop is created if someone turns on the charger without battery and then plugs in the battery
{
volt_temp=measure_volt();//check if battery detected
_delay_ms(500);//after every 500 ms
if(volt_temp>00)//if battery voltage greater than 0v,then get out of the loop
{//get out of the loop
batt=1;
}
}
 
}

//------if battery detected,check the battery status--------//

if((volt_temp<900)&&(volt_temp>0)) //if battery voltage is below 9.00v and greater than 0v
{
lcd_command(0x01);//clear the LCD
_delay_ms(10);
lcd_gotoxy(1,1);
lcd_print(" Faulty battery");//notify that it is  faulty battery
li_ion=0;//exit the charging process
batt=0;// 0 to notify a faulty battery
}


if(volt_temp>900)
{
batt=1;//proper battery detected,enter into charging mode
}


//--------if the battery is found ok,then-----------//


if(batt==1)//if the battery is connected
{

lcd_command(0x01);//clear the lcd
_delay_ms(10);
print_const1();
cc_mode=1;//now enter in CC mode
lcd_gotoxy(9,1);
lcd_print("CC");//notify cc mode via lcd
OCR2=63;//start PWM from 25%
PORTB |=(1<<1);//OPEN THE OUTPUT MOSFET

//------now in CC mode----------//

while(cc_mode==1)
{
amp_temp=measure_amp();//measure_ amp

if(amp_temp<100)
{
OCR2++;
}
else if(amp_temp>100)
{
OCR2--;
}
volt_temp=measure_volt();//now measure the voltage
print_param(4,1,volt_temp);//print the output voltage
print_param(13,2,amp_temp);//print the current
print_duty(OCR2);

_delay_ms(500);//wait 100 ms for next

PORTB &=~(1<<1);//now switch off the mosfet to measure battery voltage
_delay_ms(10);//wait a little bit
volt_temp=measure_volt();

if(volt_temp>1240)//if the battery voltage is greater than 12.4 volt
{
cc_mode=0;//exit CC mode
print_param(4,2,volt_temp);//print the voltage
lcd_gotoxy(9,1);
lcd_print("  ");//notify that we have exited cc mode via lcd
}
else
{
//continue with cc mode
print_param(4,2,volt_temp);
PORTB |=(1<<1);//again turn on the mosfet
}

}



//--------now entering CV mode---------//


cv_mode=1;
lcd_gotoxy(9,1);
lcd_print("CV");//notify cV mode via lcd
PORTB |=(1<<1);//start charge


while(cv_mode==1)
{
volt_temp=measure_volt();
//we will maintain the output voltage at 12.6v as CV mode voltage
if(volt_temp<1260)
{
OCR2++;
}
else if(volt_temp>1260)
{
OCR2--;
}

print_param(4,1,volt_temp);//print the output voltage
print_duty(OCR2);

amp_temp=measure_amp();


_delay_ms(1000);

PORTB &=~(1<<1);//now switch off the mosfet to measure battery voltage
_delay_ms(10);//wait a little bit
volt_temp=measure_volt();
print_param(4,2,volt_temp);//print battery voltage


if((amp_temp<19)&&(volt_temp>1250))//if amp drops below approx 190ma & battery voltage is greater than 12.5v
{
lcd_command(0x01);
_delay_ms(10);
lcd_gotoxy(1,1);
lcd_print("Charge complete");
cv_mode=0;//stop cv mode and complete charge cycle
batt=0;
li_ion=0;
PORTB |=(1<<2);//turn on buzzer to notify a full charge
break;
}


else
{
print_param(13,2,amp_temp);//print the current and continue with CV mode
PORTB |=(1<<1);//switch the mosfet on again
}

}

}

}




int main(void)
{
DDRB &=~(1<<0);//make PB0 input
PORTB |=(1<<0);//pull up enable
DDRB |=(1<<1)|(1<<2);//make PB1,PB2 output to drive "output" mosfet & buzzer
PORTB &=~((1<<1)|(1<<2));//make it initially off i.e no buck output voltage to battery & buzzer off

_delay_ms(1);
adc_init();
_delay_ms(1);
pwminit();
_delay_ms(1);
lcd_init();
_delay_ms(10);

OCR2=63; //(25% PWM output)

while(1)
{

if(calib_sw_pressed())//check if calibration swith is pressed
{
calibrate();
}


if(li_ion)
{
charge_li_ion();
}

}
return 0;
}