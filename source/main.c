/*	Author: kmo004
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char button = 0x00;
unsigned char Mask = 0x18;

void TimerOn()
{
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;

  _avr_timer_cntcurr = _avr_timer_M;
  
  SREG |= 0x80;
}

void TimerOff()
{
  TCCR1B = 0x00;
}

void TimerISR()
{
  TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
  _avr_timer_cntcurr--;
  if (_avr_timer_cntcurr == 0)
    {
      TimerISR();
      _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M)
{
  _avr_timer_M = M;
  _avr_timer_cntcurr = _avr_timer_M;
}

enum states{init,on,right,left} state;

unsigned char temp;

void Tick(){
	switch(state){
		case init:
			state = on;
			break;
			
		case on:
		
			if((~PINA & 0x01) == 0x01){
				state = right;
			}
			else if((~PINA & 0x02) == 0x02){
				state = left;
			}
			else{
				state = on;
			}
			break;
		
		case right:
			state = on;
			break;
		
		case left:
			state = on;
			break;
			
		default:
		state = on;
		break;
	}
	
	switch(state){
		case init:
		PORTC = ~(0x18);
		PORTD = 0x80;
		break;
		
		case on:
		break;
		
		case right:
			
				if(PORTC == 0x3F){
					break;
				}
				else{
					PORTC = (PORTC << 1);
					PORTC = PORTC + 1;
				}
		break;
		
		case left:
				if(PORTC == 0xFC){
					break;
				}
				else{
					PORTC = (PORTC >> 1);
					PORTC = PORTC - 0x80;
				}
				break;
				
		default:
		break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    TimerSet(100);
	TimerOn();

	PORTC = ~(0x18);
		PORTD = 0x80;
    /* Insert your solution below */
    while (1) {
		Tick();
		while(!TimerFlag);
      TimerFlag = 0;
      
		
		
   }
    return 1;
}

	




	
