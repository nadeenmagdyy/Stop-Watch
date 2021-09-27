/*
 * miniproject2.c
 *
 *  Created on: Sep 13, 2021
 *      Author: HERO
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char num_of_7segment = 0;

unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

void TIMER1_CTC_MODE(void)
{
	TCNT1 = 0;
	TIMSK |= (1<<OCIE1A);
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
	OCR1A = 15625; // no. of ticks needed to get 1 sec
}

ISR(TIMER1_COMPA_vect)
{
	seconds ++;
	if(seconds == 60)
	{
		minutes ++;
		seconds = 0;
	}
	if(minutes == 60)
	{
		hours ++;
		minutes = 0;
	}
}

void TIMER0_OVF_MODE(void)
{
	TCNT0 = 0;
	TCCR0 = (1<<FOC0) | (1<<CS01);
	TIMSK |= (1<<TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	if(num_of_7segment == 6)
	{
		num_of_7segment = 0;
	}
	PORTA = (PORTA & 0xC0) | (1<<num_of_7segment);
	num_of_7segment ++;

	if (PORTA & (1<<PA0))
	{
		PORTC = (PORTC & 0xF0) | (seconds%10);
	}
	else if (PORTA & (1<<PA1))
	{
		PORTC = (PORTC & 0xF0) | (seconds/10);
	}
	else if (PORTA & (1<<PA2))
	{
		PORTC = (PORTC & 0xF0) | (minutes%10);
	}
	else if (PORTA & (1<<PA3))
	{
		PORTC = (PORTC & 0xF0) | (minutes/10);
	}
	else if (PORTA & (1<<PA4))
	{
		PORTC = (PORTC & 0xF0) | (hours%10);
	}
	else if (PORTA & (1<<PA5))
	{
		PORTC = (PORTC & 0xF0) | (hours/10);
	}

}

void INT0_INIT(void)
{
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	GICR |= (1<<INT0);
	SREG |= (1<<7);
	MCUCR |= (1<<ISC01);
}

ISR(INT0_vect)
{
	//RESET
	if(!(PIND & (1<<PD2)))
	{
		_delay_ms(30);
		if(!(PIND & (1<<PD2)))
		{
			seconds = 0;
			minutes = 0;
			hours = 0;

			if(TCCR1B &= 0xF8) // check if it is paused
			{
				TCCR1B |= 0x03; // resume counting after reset
			}
		}
		while(!(PIND & (1<<PD2))) {}
	}
}

void INT1_INIT(void)
{
	DDRD &= ~(1<<PD3);
	GICR |= (1<<INT1);
	SREG |= (1<<7);
	MCUCR |= (1<<ISC11) | (1<<ISC10);
}

ISR(INT1_vect)
{
	//PAUSE
	if(PIND & (1<<PD3))
	{
		_delay_ms(30);
		if(PIND & (1<<PD3))
		{
			TCCR1B &= 0xF8; //NO CLOCK SOURCE
		}
		while(PIND & (1<<PD3)){}
	}
}

void INT2_INIT(void)
{
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	GICR |= (1<<INT2);
	SREG |= (1<<7);
	MCUCR &= ~(1<<ISC2);
}

ISR(INT2_vect)
{
	//RESUME
	if(!(PINB & (1<<PB2)))
	{
		_delay_ms(30);
		if(!(PINB & (1<<PB2)))
		{
			TCCR1B |= 0x03; //PRESCALER = 64 AGAIN AND THE TIMER RESUMES COUNTING
		}
		while(!(PINB & (1<<PB2))) {}
	}
}

int main(void)
{
	DDRC |= 0x0F;
	PORTC &= ~(0x0F);
	DDRA |= 0x3F;
	PORTA &= ~(0x3F);

	TIMER1_CTC_MODE();
	TIMER0_OVF_MODE();
	INT0_INIT();
	INT1_INIT();
	INT2_INIT();
	while(1)
	{

	}

}
