/*
* MechatronicsLab7.c
*
* Created: 4/6/2016 3:10:13 PM
* Author : James Braza
*/

#include <avr/io.h>

int sensorValue = 0; // value read from analog sensor (0 - 1023)
long delay = 8000000; //set a time to delay
long loopVar = 0; //initialize a looping variable

int main(void)
{
	DDRD = 0xFF; // Sets all pins of Port D to output.
	PORTD = 0b000000100; //Sets Port D to match the initial

	// Set up ADC
	DDRC = 0x00; // define all Port C bits as input
	PRR = 0x00; // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0; // 0x87 or 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	//enable ADC and set frequency (with a prescaler of 128)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60 or 0b01100000 // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1) and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)

	//Initialize the sensorValue a second time :)
	ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
	sensorValue = ADCH;  //Assigns sensorValue to be the high 8 bits of the 10 bit ADC converter
	
	while (1)
	{
		// Read analog input: begin ADC
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorValue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		
		
		//A time delay
		loopVar = 0;
		while (loopVar < delay)
		loopVar++;
	}
}

