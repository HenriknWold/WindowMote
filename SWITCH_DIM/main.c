/*
 * GccApplication1.c
 *
 * Created: 18.06.2018 09:56:40
 * Author : 503062581
 */ 
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int stop(void);
int Setup_timer(void);
//int Setup_PWM(void);
double n=0;
int lysstyrke=0;
unsigned volatile char dutyCycle=0;
unsigned volatile char delta=0;

int main(void)
{
	DDRA=0x00;
	DDRB= 0xff;
	delta= 200/32;
	PORTB=0xff;
	Setup_timer();
	sei();

	
	while (1)
	{
		
		if(!(PINA & 0x40))
		{
			dutyCycle= dutyCycle+delta;
			lysstyrke++;
			stop();
			
			
		}
		
		if(!(PINA & 0x80))
		{
			dutyCycle=dutyCycle-delta;
			lysstyrke--;
			stop();
		}
		
		if(lysstyrke<=0)
		{
			dutyCycle=0;
			lysstyrke=0;
		}
		
		if(lysstyrke>=32)
		{
			dutyCycle=200;
			lysstyrke=32;
		}
		//PORTB=(~lysstyrke <<  1);
	}
	
}

int Setup_timer(void)
{
	TCCR0A=(1<<WGM01); // Velger CTC i Timer Counter Control Register A
	OCR0A= 3;  // Velger Ticker verdi som skal sammenliknes (20HZ)
	TIMSK0=(1<<OCIE0A); // Velger at Interrupt Mask registert skal sammenlikne output med Match A( her velges evt overflow/
	TCCR0B= (1<<CS01)|(1<<CS00); //velger prescaler til 1024
	
}

int stop(void)
{
	_delay_ms(2);
}

ISR(TIMER0_COMPA_vect)
{
	
	n=(n+1);
	if(n<=dutyCycle)
	{
		PORTB = (~lysstyrke <<  1) | ~(1<<PB0);
	}
	else
	{
		PORTB = ( ~lysstyrke<<  1) | (1<<PB0);
	}
	if (n==200)
	{
		n=0;
	}
}

	
