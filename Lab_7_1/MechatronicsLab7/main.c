/*
* MechatronicsLab7.c
*
* Created: 4/6/2016 3:10:13 PM
* Author : James Braza
*/



// Connect motor direction pins on PD5 and PD6
// Connect potentiometer to PC0

#include <avr/io.h>

void setMotor(int PWM_speed);
long map(long x, long in_min, long in_max, long out_min, long out_max);

int sensorValue = 0; // value read from analog sensor (0 - 1023)
long delay = 8000000; //set a time to delay
long loopVar = 0; //initialize a looping variable



int main(void)
{
	DDRD = 0xFF; // Sets all pins of Port D to output.
	
	OCR0A = 0xff; // for PD6, pin12.  Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
	OCR0B = 0xff; // for PD5, pin11.  Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
	
	TCCR0A = 1<<COM0A1 | 1<<COM0B1 | 1<<WGM01 | 1<<WGM00; // Set non?inverting mode on OC0A pin (COMA1:0 and COMB0:1 bits = bits 7:4 = 1000; Fast PWM (WGM1:0 bits = bits 1:0 = 11)
	TCCR0B = 1<<CS02 | 0<<CS01 | 0<<CS00; // Set base PWM frequency (CS02:0 ? bits 2?0 = 011 for prescaler of 64, for approximately 1kHz base frequency)
	// PWM is now running on selected pin at selected duty cycle

	
	
	// Set up ADC
	DDRC = 0x00; // define all Port C bits as input
	PRR = 0x00; // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0; // 0x87 or 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	//enable ADC and set frequency (with a prescaler of 128)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60 or 0b01100000 // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1) and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)

	
	while (1)
	{
		// Read analog input: begin ADC
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorValue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		// set motor value to some mapped value from -255 to 255 baseed on sensorValue
		setMotor(map(sensorValue, 0, 255, -255, 255));

	}
}

void setMotor(int PWM_speed) {
	
	if (PWM_speed > 0){
		
		OCR0A = PWM_speed;
		OCR0B = 0;
	}
	else if( PWM_speed < 0){
		PWM_speed *= -1;
		OCR0B = PWM_speed;
		OCR0A = 0;
	}
	else {
		OCR0B = 0;
		OCR0A = 0;
	}
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	// mapping function that adjusts range
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
