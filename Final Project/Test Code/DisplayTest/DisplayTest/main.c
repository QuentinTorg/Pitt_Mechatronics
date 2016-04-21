/*
 * DisplayTest.c
 *
 * Created: 4/21/2016 12:27:27 AM
 * Author : QuentinTorgerson
 */ 

// ============================================================================

#define F_CPU 1000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

// --------------------------------------------------------------------

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                ATtiny
//               25/45/85
//             +----------+   (-)---GND--
//           --+ PB5  Vcc +---(+)---VCC--
// ---OWOWOD---+ PB3  PB2 +---------CLK--
//           --+ PB4  PB1 +----------CS--
// ------(-)---+ GND  PB0 +---------DIN--
//             +----------+
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define MAX7219_DIN		PD5	// DI,	Pin 3 on LED8x8 Board
#define MAX7219_CS		PD6	// CS,	Pin 4 on LED8x8 Board
#define MAX7219_CLK		PD7	// CLK,	Pin 5 on LED8x8 Board

#include "max7219led8x8.h"

#ifndef MAX7219_DIN
#define MAX7219_DIN		PD5	// DI,	Pin 3 on LED8x8 Board
#endif
#ifndef MAX7219_CS
#define MAX7219_CS		PD6	// CS,	Pin 4 on LED8x8 Board
#endif
#ifndef MAX7219_CLK
#define MAX7219_CLK		PD7	// CLK,	Pin 5 on LED8x8 Board
#endif

// ============================================================================



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









int main(void) {

	// ---- Initialization ----
	max7219_init();
	
	uint8_t data[8][8] = {
		{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
		{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, },
		{ 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, },
		{ 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33, },
		{ 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, },
		{ 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, },
		{ 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
	};

	// ---- Main Loop ----
	while (1) {
		for (uint8_t i = 0; i <= 7; i++) {
			for (uint8_t y = 0; y <= 7; y++) {
				uint8_t d = data[i][y];
				for (uint8_t x = 0; x <= 7; x++) {
					max7219_buffer_out();	// Output the buffer
					if ((d & 1) == 1)
					max7219_buffer_set(x, y);	// Set pixel
					else
					max7219_buffer_clr(x, y);	// Clear pixel
					d = d >> 1;
					_delay_ms(20);
				}
			}
		}
	}

	return 0;
}

