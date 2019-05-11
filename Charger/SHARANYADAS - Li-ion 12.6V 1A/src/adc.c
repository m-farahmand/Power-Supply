#include "adc.h"


void adc_init(void)
{
DDRC=0x00;//make adc port input
ADCSRA |=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//ADC enabled,clk/128 frequency
}

uint16_t read_adc_raw(uint8_t ch)
{
ch=ch & 0b00000111;
ADMUX =ADCREF_AREF | ch;//this is basically updating the channel bits every time when we selecting channel.
ADCSRA |=(1<<ADSC);//start adc conversion
while((ADCSRA &(1<<ADIF))==0);//stay until ADIF becomes 1 to indicate finish conversion
_delay_us(1);
return(ADC);//return ADCH+ADCL.In avr gcc,it is denoted as ADC register
}


uint16_t read_adc_avg(uint8_t ch,uint16_t sample)
{
unsigned char i;
unsigned int r[sample];//to store multiple reading.No of samples is defined by the user as per criteria
unsigned int avg;//to store the average result
unsigned long int sum=0;//to store the sums of the readings and must be initialized as 0 to prevent errors

for(i=0;i<sample;i++)
{
r[i]=read_adc_raw(ch);
sum+=r[i];
_delay_ms(1);//delay between each readout of ADC
}
avg=sum/sample;
return(avg);
}


/*****************This functions are here to show how to use above mentioned functions as example***********************


uint16_t measure_volt(uint8_t ch,uint16_t sample) This function will measure volt with a simple voltage divider.ref_volt=5.00 as the ADC is using 5v reference
//div=(R1+R2)/R2
{
unsigned int adc_avg;
unsigned int volt;
float temp_val;

adc_avg=read_adc_avg(ch,sample);
temp_val=(adc_avg * ref_volt * div)/1023;
volt=(unsigned int)(temp_val * 100);
return volt;
}



uint16_t measure_amp(uint8_t ch,uint16_t sample) This function will measure AMP using ACS714 cutrrent sensor
{
float temp_val;
unsigned int adc_val;
unsigned int amp=0;
adc_val=read_adc_avg(ch,sample);
if(adc_val>512)
{
temp_val=(adc_val*ref_volt)/1023;
temp_val=(temp_val-2.51)/0.185;
amp=(unsigned int)(temp_val*100);
}
return amp;
}



uint8_t read_temp(uint8_t ch,uint16_t sample) This function will measure temparature using LM35
{
uint16_t temp_val1;
uint8_t temp1;

temp_val1=read_adc_avg(ch,sample);
temp1=temp_val1/2;
return temp1;
}
*/