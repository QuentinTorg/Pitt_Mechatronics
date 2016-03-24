/*
* MechatronicsLab5.c
*
* Created: 3/24/2016 12:16:40 PM
* Author : James Braza
*/

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t count = 0; //unsigned 8 bit integer called count
uint8_t interruptFlag = 0;

void delayNms_timer0();
void wait(int msec);

int main(void)
{	// Setup
	//input = 0, output = 1
	
	DDRC = 0x0F;  //LED's: set bits PC7-PC0 as output (all output)
	DDRD = 0x00; //Buttons: set bits PD7-PD0 as input (all input)
	
	//PORTD = 1<<PD2 | 1<<PD3; //enables pull-up resistors

	EICRA = 1<<ISC01 | 0<<ISC00 | 1<<ISC11 | 0<<ISC10;// Trigger INT0 on rising edge; Trigger INT1 on rising edge
	EIMSK = 1<<INT1 | 1<<INT0;// Enable INT1 and INT0
	sei(); //Enable Global Interrupt

	while (1) {
		//temp = PIND; // "Read" the contents of PortD (the pins set as inputs)
		//PORTB = temp; // "Write" the contents of R16 to PortB (Note that pins PB3, PB4, and PB5 are also used for in-circuit programming
		//so they may not work properly if the programmer is still connected.)
		
		//0 means LED's turn on - active low
		
		if (interruptFlag)
		{
			wait(500);
			count++;
			interruptFlag = 0;
		}

		PORTC = ~count;
		wait(250);

		count++;
		if (count == 0)
		{count = 15;}
	}

	return(0);
}

void wait(int msec) {// function passes number of msec to delay
	while (msec > 0) {
		delayNms_timer0();
		msec = msec - 1;
	}
}

void delayNms_timer0() {
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 5; // load timer0 with number for delay (count must reach 255-5)
	//TCCR0B = 0x01; // Start Timer0, Normal mode, crystal clock, no prescaler
	TCCR0B = 0x02; // Start Timer0, Normal mode, crystal clock, prescaler = 8
	//TCCR0B = 0x03; // Start Timer0, Normal mode, crystal clock, prescaler = 64
	//TCCR0B = 0x04; // Start Timer0, Normal mode, crystal clock, prescaler = 256
	//TCCR0B = 0x05; // Start Timer0, Normal mode, crystal clock, prescaler = 1024
	while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// For an explanation of this statement, see the notes below.
	TCCR0B = 0x00; // Stop Timer0
	TIFR0 = 0x1<<TOV0; // Clear TOV0 (note that this is an odd bit in that it is cleared by writing a 1 to it)
}

//Interrupt Service Routine for INT0
ISR(INT0_vect)
{
	/*unsigned int timer=0;
	PORTC=0x00;
	while (timer < 1000000)
	{
	timer++;
	}*/
	interruptFlag = 1;
}

//Interrupt Service Routine for INT1
ISR(INT1_vect)
{
	/*PORTC = count;
	wait(250);

	count--;	
	if (count == 0)
	{count = 15;}*/
}
