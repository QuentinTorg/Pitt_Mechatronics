/*
* MechatronicsFinalFFTandLEDMatric.c
*
* Created: 4/21/2016 3:20:33 AM
* Author : James Braza
*/

#include <avr/io.h>
#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>
//#include "max7219led8x8.h"
#include <stdlib.h>

//

#define MAX7219_DIN		PD5	// DI,	Pin 3 on LED8x8 Board
#define MAX7219_CS		PD6	// CS,	Pin 4 on LED8x8 Board
#define MAX7219_CLK		PD7	// CLK,	Pin 5 on LED8x8 Board

#ifndef MAX7219_DIN
#define MAX7219_DIN		PD5	// DI,	Pin 3 on LED8x8 Board
#endif
#ifndef MAX7219_CS
#define MAX7219_CS		PD6	// CS,	Pin 4 on LED8x8 Board
#endif
#ifndef MAX7219_CLK
#define MAX7219_CLK		PD7	// CLK,	Pin 5 on LED8x8 Board
#endif

#define HIGH 1
#define LOW  0

void delay_ms(uint16_t count);
void delay_us(uint16_t count);
void digitalWritePortD(uint8_t pin, uint8_t val);

uint8_t portD_value = 0b00000000; //The PortD variable for digitalWrite
int sensorValue = 0; // value read from analog sensor (0 - 1023)
long delay = 36000; //set a time to delay
long loopVar = 0; //initialize a looping variable

const int ANALOG = 0; //PC0
const int STROBE = 0; //PD0
const int RESET = 1; //PD1
int outputValues[7];

int main(void)
{
	max7219_init();
	
	DDRD = 0xFF; // Sets all pins of Port D to output.
	DDRC = 0x00; // Sets all pins of Port C to input.
	
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

	//Initialize the sensorValue a second time :)
	ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
	sensorValue = ADCH;  //Assigns sensorValue to be the high 8 bits of the 10 bit ADC converter
	
	digitalWritePortD(RESET, LOW); //Turns off the reset
	digitalWritePortD(STROBE, HIGH); //Turns on the strobe
	
	while (1)
	{
		
		digitalWritePortD(RESET, HIGH); //Sets the reset
		delay_us(1);
		digitalWritePortD(RESET, LOW); //Turns off the reset
		delay_us(100);

		for (int i = 0; i < 7; i++)
		{
			digitalWritePortD(STROBE, LOW);
			delay_us(50); //A time delay of 30 micro seconds
			
			// Read analog input: begin ADC
			ADCSRA |= (1<<ADSC); // Start conversion
			while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
			sensorValue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
			//ADC Complete
			
			outputValues[i] = sensorValue/32.0;
			delay_us(50);
			digitalWritePortD(STROBE, HIGH);
			delay_us(40);
		}
		
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				max7219_buffer_out();	// Output the buffer
				
				if (j < outputValues[i])
				{
					max7219_buffer_set(i, j);	// Set pixel
				}
				else
				{
					max7219_buffer_clr(i, j);	// Clear pixel
				}
				//_delay_us(5);
			}
		}
		
		
	}
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

void digitalWritePortD(uint8_t pin, uint8_t val) {
	if (val != 0) {
		portD_value = (portD_value) | (1<<pin);
	}
	else {
		portD_value = (portD_value) & (0<<pin);
	}
	PORTD = portD_value;
}

void max7219_byte(uint8_t data)
{
	PORTD &= ~(1 << MAX7219_CS);	// Set to LOW
	for(uint8_t i = 8; i >= 1; i--)
	{
		PORTD &= ~(1 << MAX7219_CLK);	// Set to LOW
		if ((data & 0x80) != 0)			// Mask the MSB of the data
		PORTD |= (1 << MAX7219_DIN);	// Set to HIGH
		else
		PORTD &= ~(1 << MAX7219_DIN);	// Set to LOW
		data = data<<1;
		PORTD |= (1 << MAX7219_CLK);		// Set to HIGH
	}
}

void max7219_word(uint8_t address, uint8_t data)
{
	PORTD &= ~(1 << MAX7219_CS);	// Set to LOW
	max7219_byte(address);			//
	max7219_byte(data);				//
	PORTD |= (1 << MAX7219_CS);		// Set to HIGH
}

void max7219_init(void)
{
	DDRD |= (1 << MAX7219_CLK);	// Set port as output
	DDRD |= (1 << MAX7219_CS);	// Set port as output
	DDRD |= (1 << MAX7219_DIN);	// Set port as output
	_delay_ms(50);	// TODO: Q: Is this necessary?
	max7219_word(0x09, 0x00);	// Decode: BCD
	max7219_word(0x0a, 0x03);	// Brightness
	max7219_word(0x0b, 0x07);	//
	max7219_word(0x0c, 0x01);	//
	max7219_word(0x0f, 0x00);	//
}

void max7219_row(uint8_t address, uint8_t data) {
	if (address >= 1 && address <= 8) max7219_word(address, data);
}

uint8_t max7219_buffer[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void max7219_buffer_out(void) {
	// Output the buffer
	for (uint8_t row = 1; row <= 8; row++)
	max7219_row(row, max7219_buffer[row - 1]);
}

void max7219_buffer_set(uint8_t x, uint8_t y) {
	uint8_t sx = 7 - (x & 0b0111);
	uint8_t sy = (y & 0b0111);
	max7219_buffer[sy] |= (1 << sx);
}

void max7219_buffer_clr(uint8_t x, uint8_t y) {
	uint8_t sx = 7 - (x & 0b0111);
	uint8_t sy = (y & 0b0111);
	max7219_buffer[sy] &= ~(1 << sx);
}

void max7219_buffer_row(uint8_t row, uint8_t y) {
	max7219_buffer[y & 0b0111] = row;
}