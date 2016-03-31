/*
 * GccApplication2.c
 * Mechatronics Lab 6 Code
 * Created: 3/31/2016 1:31:44 PM
 * Author : James Braza
 */ 

#include <avr/io.h>

uint8_t sensorValue = 0; // value read from analog sensor (0-1023)
uint8_t stepperAngle; //The angle the stepper should go to
//uint8_t Vref = 1023; //Vref = 5V

/* Our Stepper Motor Pins
PD2 = 1in, PD3 = 2in, PD4 = 3in, PD5 = 4in;
1in + 2in = Coil 1, 3in + 4in = Coil 2

Stepper Motor Specs
int steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec*/

int stepsPerRev = 200;

int main(void)
{
    DDRD = 0xFF; // Sets all pins of Port D to output.
	//DDRB = 0xFF; // Sets all pins of Port B to output.
	
	// Set up ADC
	DDRC = 0x00; // define all Port C bits as input
	PRR = 0x00; // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0; // 0x87 or 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	//enable ADC and set frequency (with a prescaler of 128)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60 or 0b01100000 // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1);
	//and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)
	
    while (1) 
    {
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorValue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//PORTD = sensorvalue; // echo results back out PORTD
		stepperAngle = 0.3519 * sensorValue; //Computes the angle the stepper needs to be currently positioned at, (0V = 0b00000000, 0 Degrees), (5V = 0v11111111, 360 Degrees)
		//PORTD = potAngle; Sets Port D to the angle of the stepper
    }
	
	return(0);
}