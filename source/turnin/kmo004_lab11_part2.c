/*	Author: kmo004
 *  Partner(s) Name: Michael Wen
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
/*	Author: kmo004
 *  Partner(s) Name: Michael Wen
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


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    unsigned short time = 500;
    unsigned char temp = 0;
    unsigned char n = 38;
    unsigned char i = 0;
    TimerSet(time);
	TimerOn();
    
    LCD_init();
    unsigned char MSG[] = {'C','S','1','2','B',' ','i','s',' ', 'L','e','g','e','n','d',' ','.','.','.',' ','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!',' '};

    /* Insert your solution below */
    while (1) {
		LCD_DisplayString(1, MSG);
	  while(!TimerFlag);
      TimerFlag = 0;
      
      temp = MSG[0];
      for(i = 0; i < (n-1); i++){
		  MSG[i] = MSG[i+1];
	  }
	  MSG[n-1] = temp;
   }
    return 1;
}

	
