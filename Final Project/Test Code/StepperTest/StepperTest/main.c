/*
 * StepperTest.c
 *
 * Created: 4/19/2016 4:45:40 PM
 * Author : QuentinTorgerson
 */ 

#include <avr/io.h>

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>

#define HIGH 1
#define LOW  0

//Function Definitions
int updateStepper(int stepDesired);
void delay_ms(uint16_t count);
void delay_us(uint16_t count);

// pin assignment cheater functions
void digitalWritePortB(uint8_t pin, uint8_t val);
void digitalWritePortC(uint8_t pin, uint8_t val);
void digitalWritePortD(uint8_t pin, uint8_t val);

//Global variables
int sensor_value = 0; // value read from analog sensor (0 - 1023)
int step_current = 0; //Variables corresponding to the stepper's steps
int desired_location = 0;
long delay_between_steps = 5; //set a time to delay between steps
int loops_per_milli = 1;

uint8_t portB_value = 0b00000000;
uint8_t portC_value = 0b00000000;
uint8_t portD_value = 0b00000000;

uint8_t step_pin = 2;
uint8_t direction_pin = 3;

/* Our Stepper Motor Pins
PD2 = step
PD3 = direction

Stepper Motor Specs
int steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec*/

int main(void)
{
	DDRD =	0b000101100; // Sets all pins of Port D to output.
	PORTD = 0b000000000; //Sets Port D to match the initial
	DDRB =	0b000000010;
	
	//Configure TIMER1
	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS12); //PRESCALER=256 MODE 14(FAST PWM)
	ICR1=4999;  //fPWM=50Hz (Period = 20ms Standard).
	
	
	// Set up ADC
	DDRC = 0x00; // define all Port C bits as input
	PRR = 0x00; // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0; // 0x87 or 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	//enable ADC and set frequency (with a prescaler of 128)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60 or 0b01100000 // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1) and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)
	
	
	while (1)
	{
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensor_value = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		desired_location = sensor_value * 50; //The times four comes from we needed more steps per degree from potentiometer
		
		//Stepper Stuff
		updateStepper(desired_location);
		
		//A time delay to accommodate out stepper
		delay_us(delay_between_steps);
		
		
		
		//OCR1A=45;   //0 degree
		//delay_ms(1000);

		//OCR1A=92;  //90 degree
		//delay_ms(5000);

		//OCR1A=145;  //135 degree
		//delay_ms(1000);


	}
	
	return(0);
}

int updateStepper(int step_desired)
{
	int adder_var = 0;
	
	if (step_desired > step_current)
	{
		digitalWritePortD(direction_pin, HIGH);
		adder_var = 1;
	}
	else if (step_desired < step_current)
	{
		digitalWritePortD(direction_pin, LOW);
		adder_var = -1;
	}

	if (adder_var != 0)
	{
		// write pin high then low for a single pulse
		digitalWritePortD(step_pin, HIGH);
		digitalWritePortD(step_pin, LOW);
	}
	// increment current position
	step_current += adder_var;
	// return current position in case required for a wait function
	return step_current;
}






void digitalWritePortB(uint8_t pin, uint8_t val) {
	if (val != 0) {
		// if the value is non-zero, use the OR operator
		portB_value = (portB_value) | (1<<pin);
	}
	else {
		// if the value is zero, use the AND operator
		portB_value = (portB_value) & (0<<pin);
	}
	// set PORTB to the portB_value 
	PORTB = portB_value;
}
void digitalWritePortC(uint8_t pin, uint8_t val) {
	if (val != 0) {
		portC_value = (portC_value) | (1<<pin);
	}
	else {
		portC_value = (portC_value) & (0<<pin);
	}
	PORTC = portC_value;
}
void digitalWritePortD(uint8_t pin, uint8_t val) {
	if (val != 0) {
		portD_value = (portD_value) | (1<<pin);
	}
	else {
		portD_value = (portD_value) & (0<<pin);
	}
	PORTD = portD_value;
}



void delay_ms(uint16_t count) {
	while(count--) {
		_delay_ms(1);

	}
}

void delay_us(uint16_t count) {
	while(count--) {
		_delay_us(1);

	}
}

