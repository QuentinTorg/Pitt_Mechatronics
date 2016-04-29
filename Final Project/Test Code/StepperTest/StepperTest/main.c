/*
* StepperTest.c
*
* Created: 4/19/2016 4:45:40 PM
* Author : Quentin Torgerson and James Braza
*/

#include <avr/io.h>

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>

#define OFF 2
#define HIGH 1
#define LOW  0

//Function Definitions
void updateStepper(long stepDesired);
void delay_ms(long count);
void delay_us(long count);
void playFlute(long playSound);
long noteLookup(char note);
void driveShark(long sharkDirection);

// pin assignment cheater functions
void digitalWritePortB(uint8_t pin, uint8_t val);
void digitalWritePortC(uint8_t pin, uint8_t val);
void digitalWritePortD(uint8_t pin, uint8_t val);



//Global variables
long sensor_value = 0; // value read from analog sensor (0 - 1023)
long step_current = 0; //Variables corresponding to the stepper's steps
long desired_location = 0;
long delay_between_steps = 10; //set a time to delay between steps
long loops_per_milli = 1;
float accel_multiplier = 1.17;

//Global variables for updateStepper snippets

long max_velocity = 50;
long start_velocity = 5000;

uint8_t portB_value = 0b00000000;
uint8_t portC_value = 0b00000000;
uint8_t portD_value = 0b00000000;

uint8_t step_pin = 2;
uint8_t direction_pin = 3;
uint8_t mot_Pin_Left = 6;
uint8_t mot_Pin_Right = 7;
uint8_t sharkMot_Pin_Left = 4;
uint8_t sharkMot_Pin_Right = 5;
uint8_t communication_signal_pin = 1;

/* Our Stepper Motor Pins
PD2 = step
PD3 = direction

Stepper Motor Specs
long steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec*/

long main(void)
{
	DDRD =	0b11111110; // Sets pins of Port D according to circuit
	//Stepper Pins: sets PD2 = step pin and PD3 = direction pins
	//Shark Motor Pins: sets PD4 = left pin and PD5 = right pin for the H bridge
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
	
	while (1)
	{
		playFlute(HIGH);  //Starts the flute out as closed
		/*
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensor_value = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		//ADC Complete
		
		desired_location = sensor_value * 50; //The times four comes from we needed more steps per degree from potentiometer
		*/

		while (PINB & (1<<PB0) == 1);
		
		
		delay_ms(2000);

		
		driveShark(HIGH);
		

		updateStepper(noteLookup('G'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(150);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('A'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(150);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('a'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(400);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);
		delay_ms(50);
		
		
		//updateStepper(noteLookup('a'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(350);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('F'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(200);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('d'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(300);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('w'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(270);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);

		
		updateStepper(noteLookup('D'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(225);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);
		delay_ms(50);
		
		//updateStepper(noteLookup('D'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(225);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);
		delay_ms(50);
		
		//updateStepper(noteLookup('D'));
		playFlute(LOW);
		digitalWritePortD(communication_signal_pin,HIGH);
		delay_ms(500);
		digitalWritePortD(communication_signal_pin,LOW);
		playFlute(HIGH);
		
		
		driveShark(OFF);
		delay_ms(500);
		updateStepper(0);

		
	}
	return 0;
}

void driveShark(long sharkDirection) {
	if (sharkDirection == HIGH) {
		digitalWritePortD(sharkMot_Pin_Right, LOW);
		digitalWritePortD(sharkMot_Pin_Left, HIGH);
	}
	else if (sharkDirection == LOW) {
		digitalWritePortD(sharkMot_Pin_Left, LOW);
		digitalWritePortD(sharkMot_Pin_Right, HIGH);
	}
	else if (sharkDirection == OFF) { //Turns the Shark off
		digitalWritePortD(sharkMot_Pin_Left, LOW);
		digitalWritePortD(sharkMot_Pin_Right, LOW);
	}
}

long noteLookup(char note) {
	float length = 0.0; //lengths are in mm
	long steps = 0; //steps is an integer corresponding to how many steps the stepper moves
	// MOTOR at 7.5 volts
	
	switch (note) {
		case 'w' : //w = = A# = Bflat but the octave lower
		length = 32,37;
		break;
		case 'd':
		length = 50.02;
		break;
		case 'F' :
		length = 0;
		break;
		case 'G' :
		length = 14.5;
		break;
		case 'A' :
		length = 26.84;
		break;
		case 'a' : //a = A# = Bflat
		length = 32.37;
		break;
		case 'B' :
		length = 37.12;
		break;
		case 'D' :
		length = 50.02;
		break;
		
		default: //if there is an unknown note, the motor does not step
		return 0;
	}
	
	return steps = (4000.0 / 112.75) * length;
}

void updateStepper(long step_desired)
{
	//initializes some variables to zero
	long step_start = step_current;
	long accel_distance = 0;
	long accel_sign = 0; //initially the acceleration is neither forward nor backward
	long distance_from_start = 0;
	long velocity_sign = 0;
	long current_velocity = start_velocity;
	long steps_to_max_velocity = 0;
	long adder_var = 0;
	long flipped_accel_flag = 1;

	playFlute(HIGH);

	//Gives velocity and acceleration signs
	if (step_desired > step_current) {
		accel_sign = 1;
		velocity_sign = 1;
		digitalWritePortD(direction_pin, HIGH);
		adder_var = 1;
	}
	else if (step_desired < step_current) {
		accel_sign = -1;
		velocity_sign = -1;
		digitalWritePortD(direction_pin, LOW);
		adder_var = -1;
	}
	else { //if your current step = the desired step
		return;
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
		else if ((steps_to_max_velocity > abs(step_current - step_desired)) & (flipped_accel_flag != 0)){
			accel_sign = -accel_sign;
			flipped_accel_flag = 0;
		}

	}
	return;
}

/* Improved update Stepper Code Snippets
long updateStepper(long step_desired)
{
long current_delay = start_delay;
long delay_sign = 0;
long accel_sign = 0;
long adder_var = 0;
long steps_to_max_velocity = 0;

if (step_desired > step_current) {
accel_sign = 1;
delay_sign = 1;
digitalWritePortD(direction_pin, HIGH);
adder_var = 1;
}
else if (step_desired < step_current) {
accel_sign = -1;
delay_sign = -1;
digitalWritePortD(direction_pin, LOW);
adder_var = -1;
}
else {
return step_current;
}

while (step_current != step_desired) {

// send pulse
digitalWritePortD(step_pin, HIGH);
digitalWritePortD(step_pin, LOW);

// increment current position
step_current += adder_var;
delay_us(current_delay);

if (delay_sign == accel_sign) {
current_delay = current_delay / accel_multiplier; // Accelerate
}
else {
current_delay = current_delay * accel_multiplier; // Slow down
}

if (current_delay < min_delay) {
current_delay = min_delay;
}

// determine when to start slowing down
if ((current_delay != max_delay) && (accel_sign == velocity_sign) && (steps_to_max_velocity < abs(step_current - step_desired))) {
steps_to_max_velocity++;
}
else if (steps_to_max_velocity > abs(step_current - step_desired)){
accel_sign = -accel_sign;
}
}

return step_current;
}
*/

void playFlute(long playSound)
{
	if (playSound == HIGH) { //Tells the motor to up and snuff sound
		digitalWritePortD(mot_Pin_Right, LOW);
		digitalWritePortD(mot_Pin_Left, HIGH);
	}
	else if (playSound == LOW) {
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

void delay_ms(long count) {
	while(count--) {
		_delay_ms(1);
	}
}

void delay_us(long count) {
	while(count--) {
		_delay_us(1);
	}
}