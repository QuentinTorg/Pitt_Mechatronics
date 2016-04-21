/*
* StepperTest.c
*
* Created: 4/19/2016 4:45:40 PM
* Author : Quentin Torgerson and James Braza
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
void playFlute(int playSound);

//Global variables
int sensor_value = 0; // value read from analog sensor (0 - 1023)
int step_current = 0; //Variables corresponding to the stepper's steps
int desired_location = 0;
long delay_between_steps = 5; //set a time to delay between steps
int loops_per_milli = 1;
float accel_multiplier = 1.0010;

int max_velocity = 50;
long start_velocity = 100000000;

uint8_t portB_value = 0b00000000;
uint8_t portC_value = 0b00000000;
uint8_t portD_value = 0b00000000;

uint8_t step_pin = 2;
uint8_t direction_pin = 3;
uint8_t mot_Pin_Left = 6;
uint8_t mot_Pin_Right = 7;

/* Our Stepper Motor Pins
PD2 = step
PD3 = direction

Stepper Motor Specs
int steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec*/

int main(void)
{
	DDRD =	0b11001100; // Sets pins of Port D according to circuit
	//Stepper Pins: sets PD2 = step pin and PD3 = direction pins
	//Motor Pins: sets PD6 = left pin and PD7 = right pin for the H bridge
	PORTD = 0b00000000; //Sets Port D to match the initial of all zeros
	DDRB =	0b00000010; // Sets PB1 as output, sets the rest as input
	
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
	
	playFlute(LOW);
	
	while (1)
	{
		/*
		//Toggle On/Off Motor
		playFlute(LOW);
		delay_ms(500);
		playFlute(HIGH);
		delay_ms(500);
		*/
		
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensor_value = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		desired_location = sensor_value * 50; //The times four comes from we needed more steps per degree from potentiometer
		
		//Stepper Stuff
		updateStepper(5000);
		
	}
	
	return(0);
}

/*
int updateStepper(int step_desired)
{
	int velocity_sign = 0;
	int accel_sign = 0;
	int adder_var = 0;
	
	if (step_desired > step_current) {
		accel_sign = 1;
		velocity_sign = 1;
		digitalWritePortD(direction_pin, HIGH);
		adder_var = 1;
	}
	else if (step_desired < step_current) {
		
	}
}*/

int updateStepper(int step_desired)
{
	//initializes some variables to zero
	int step_start = step_current;
	int accel_distance = 0;
	int accel_sign = 0; //initially the acceleration is neither forward nor backward
	int distance_from_start = 0;
	int velocity_sign = 0;
	int current_velocity = start_velocity;
	int steps_to_max_velocity = 0;
	
	//Gives velocity and acceleration signs
	if (step_desired > step_current) {
		accel_sign = 1;
		velocity_sign = 1;
	}
	else if (step_desired < step_current) {
		accel_sign = -1;
		velocity_sign = -1;
	}
	else { //if your current step = the desired step
		return step_current;
	}
	
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
	
	while (step_current != step_desired)
	{
		if (adder_var != 0) //If the motor is not at the desired step, write step pin high then low for a single pulse
		{
			digitalWritePortD(step_pin, HIGH);
			digitalWritePortD(step_pin, LOW);
		}
		
		// increment current position
		step_current += adder_var;
		delay_us(current_velocity);
		
		if (velocity_sign == accel_sign) {
			current_velocity = current_velocity / accel_multiplier;
		}
		else {
			current_velocity = current_velocity * accel_multiplier;
		}
		
		if (current_velocity < max_velocity) {
			current_velocity = max_velocity;
		}
		
		if ((current_velocity != max_velocity) && (accel_sign == velocity_sign) && (steps_to_max_velocity < abs(step_current - step_desired))) {
			steps_to_max_velocity++;
		}
		else if (steps_to_max_velocity > abs(step_current - step_desired)){
			accel_sign = -accel_sign;
		}

	}
	return step_current;
}

void playFlute(int playSound)
{
	if (playSound == LOW) { //Tells the motor to go CW
		digitalWritePortD(mot_Pin_Right, LOW);
		digitalWritePortD(mot_Pin_Left, HIGH);
	}
	else if (playSound == HIGH) {
		digitalWritePortD(mot_Pin_Left, LOW);
		digitalWritePortD(mot_Pin_Right, HIGH);
	}
}

void digitalWritePortB(uint8_t pin, uint8_t val) {
	if (val != 0) {
		// if the value is non-zero, use the OR operator
		portB_value |= (1<<pin);
	}
	else {
		// if the value is zero, use the AND operator
		portB_value &= ~(1<<pin);
	}
	// set PORTB to the portB_value
	PORTB = portB_value;
}
void digitalWritePortC(uint8_t pin, uint8_t val) {
	if (val != 0) {
		portC_value |= (1<<pin);
	}
	else {
		portC_value &= ~(1<<pin);
	}
	PORTC = portC_value;
}
void digitalWritePortD(uint8_t pin, uint8_t val) {
	if (val != 0) {
		portD_value |= (1<<pin);
	}
	else {
		portD_value &= ~(1<<pin);
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