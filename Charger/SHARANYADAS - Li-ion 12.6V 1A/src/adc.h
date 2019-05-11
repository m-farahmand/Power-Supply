//This library is createdto use the internal ADC of atmega

#ifndef ADC_H
#define ADC_H

#define ADCREF_AREF 0X00 //ADC reference voltage is set externally via Aref pin 
#define ADCREF_AVCC 0x40 //REFS0=1,aref=avcc internally,put cap from avcc to agnd,aref to agnd,connect avcc to vcc

#include<avr/io.h>
#include<util/delay.h>

void adc_init(void);
uint16_t read_adc_raw(uint8_t ch);
uint16_t read_adc_avg(uint8_t ch,uint16_t sample);

#endif