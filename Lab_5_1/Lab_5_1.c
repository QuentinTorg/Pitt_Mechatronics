/*
* GccApplication1.c
*
* Created: 3/25/2016 2:23:59 PM
* Author : James, Henry, and Quentin
*/

#include <avr/io.h>
#include <avr/interrupt.h>

//** Functions **//
void wait(int msec);
void delayNms_timer0();
uint8_t count = 1;

//Interrupt Service Routine for INT0
ISR(INT0_vect)
{
	//TCCR0B = 0x00; // Stop TIMER0
	long time = 0;
	
	//EIMSK = 0<<INT0;
	int count_temp = count;
	for (int i = 0;  i < 16; i++ )
	{
		while (time <80000)
		{
			time++;
		}
		time = 0;
		count_temp++;
		PORTC = ~count_temp;
	}
	
	
	//for (int time = 0; time<160000; time++) {};
	
	//EIMSK = 1<<INT0;
	//sei(); //Enable Global Interrupt
	EIFR = 1<<INTF0;
	//TCCR0B = 0x02;
}

//Interrupt Service Routine for INT1
ISR(INT1_vect)
{	
	//TCCR0B = 0x00;
	long time = 0;
	//EIMSK = 0<<INT1;
	
	PORTC = ~0b00000000;
	while (time <160000)
	{
		time++;
	}
	time = 0;
	PORTC = ~0b00001001;
	while (time <160000)
	{
		time++;
	}
	time = 0;
	PORTC = ~0b00001111;
	while (time <160000)
	{
		time++;
	}
	time = 0;
	PORTC = ~0b00000110;
	while (time <160000)
	{
		time++;
	}
	time = 0;
	PORTC = ~0b00000000;
	while (time <160000)
	{
		time++;
	}
	time = 0;
	
	
	//EIMSK = 1<<INT1;
	//TCCR0B = 0x02;
	//sei(); //Enable Global Interrupt
	EIFR = 1<<INTF1;
}

int main(void)
{
	DDRD = 0<<PD2 | 0<<PD3;// Set PD2 and PD3 as input (For use as interrupts INT0 and INT1)
	//PORTD = 1<<PD2 | 1<<PD3;// Enable PD2 and PD3 pull-up resistors
	DDRC = 0b11111111; // Set PORTC as output
	EICRA = 1<<ISC01 | 1<<ISC00 | 1<<ISC11 | 1<<ISC10;// Trigger INT0 on rising edge; Trigger INT1 on falling edge
	EIMSK = 1<<INT1 | 1<<INT0;// Enable INT1 and INT0
	sei(); //Enable Global Interrupt
	while(1)
	{
		PORTC = ~count;
		wait(250);
		if (count >= 8){
			count = 1;
		}
		else {
			count= count*2;
		}
		
		// The main loop -- create some changing display on the LEDs attached to PORTC
	}
	return(0);
}

void wait(int msec) {
	while (msec > 0) {
		delayNms_timer();
		msec = msec - 2;
	}
}

void delayNms_timer() {
	TCCR0A = 0;
	TCNT0 = 5;
	
	//TCCR0B = 1<<CS01 | 1<< CS00;
	TCCR0B = 0x02;
	
	while ((TIFR0 & (0x1<<TOV0)) == 0);
	
	TCCR0B = 0;
	TIFR0 = 1<<TOV0;
}