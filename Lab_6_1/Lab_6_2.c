/*
* GccApplication2.c
* Mechatronics Lab 6 Code
* Created: 3/31/2016 1:31:44 PM
* Author : James Braza, Quentin Torgerson, Henry Russell
*/

#include <avr/io.h>

//Function Definitions
void stepAssign(int stepMod);
int comparator(int valCurrent, int valDesired);

//Global variables
int sensorValue = 0; // value read from analog sensor (0 - 1023)
int stepCurrent, stepDesired; //Variables corresponding to the stepper's steps
long delay = 1; //set a time to delay
long loopVar; //initialize a looping variable
float stepPerDeg = 200/360; //a function of steps per rev and degrees (200/360)
int a = 0; //Debugging variable

/* Our Stepper Motor Pins
PD2 = 1in, PD3 = 2in, PD4 = 3in, PD5 = 4in;
1in + 2in = Coil 1, 3in + 4in = Coil 2

Stepper Motor Specs
int steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec*/

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

	//Define stepCurrent
	ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
	sensorValue = ADCH;  //Assigns sensorValue to be the high 8 bits of the 10 bit ADC converter
	
	//initialize the current step variable
	stepCurrent = 0.1953125 * sensorValue * 4 - 100.0; //The times four comes from we needed more steps per degree from potentiometer
	stepAssign(stepCurrent % 4);  // % means modular divide, can also use math.h library's modular divide
	
	while (1)
	{
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorValue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		stepDesired = 0.1953125 * sensorValue * 4 - 100.0; //The times four comes from we needed more steps per degree from potentiometer
		
		//Stepper Stuff
		stepCurrent += comparator(stepCurrent, stepDesired); //direction is a uint8_t corresponding to which way the stepper should go
		stepAssign(stepCurrent % 4);
		
		//A time delay to accommodate out stepper
		loopVar = 0;
		while (loopVar < delay)
		loopVar++;
	}
	
	return(0);
}

void stepAssign(int stepMod)
{
	if (stepMod == 0)
	{
		PORTD = 0b000000100;
	}
	else if (stepMod == 1 || stepMod == -3)
	{
		PORTD = 0b000100000;
	}
	else if (stepMod == 2 || stepMod == -2)
	{
		PORTD = 0b000001000;
	}
	else if (stepMod == 3 || stepMod == -1)
	{
		PORTD = 0b000010000;
	}
}

int comparator(int valCurrent, int valDesired) //returns the direction the stepper needs to go in (-1 = CCW, 0 = do nothing, 1 = CW))
{
	if (valCurrent < valDesired)
	{
		return 1;
	}
	else if (valCurrent > valDesired)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}