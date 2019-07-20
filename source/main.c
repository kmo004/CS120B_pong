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

void TimerOn(){
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;

  _avr_timer_cntcurr = _avr_timer_M;
  
  SREG |= 0x80;
}

void TimerOff(){
  TCCR1B = 0x00;
}

void TimerISR(){
  TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
  _avr_timer_cntcurr--;
  if (_avr_timer_cntcurr == 0)
    {
      TimerISR();
      _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
  _avr_timer_M = M;
  _avr_timer_cntcurr = _avr_timer_M;
}

enum states{init,on,right,left} state;
unsigned char temp;
volatile unsigned char PaddleC = 0x00;
volatile unsigned char PaddleR = 0x00;

void TickPaddle(){
	switch(state){
		case init:
			state = on;
			break;
			
		case on:
		
			if((~PINB & 0x01) == 0x01){
				state = right;
			}
			else if((~PINB & 0x02) == 0x02){
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
		state = init;
		break;
	}
	
	switch(state){
		case init:
		PaddleC = ~(0x18);
		PaddleR = 0x80;
		break;
		
		case on:
		break;
		
		case right:
				if(PaddleC == 0x3F){
					break;
				}
				else{
					PaddleC = (PaddleC << 1);
					PaddleC = PaddleC + 1;
				}
		break;
		
		case left:
				if(PaddleC == 0xFC){
					break;
				}
				else{
					PaddleC = (PaddleC >> 1);
					PaddleC = PaddleC - 0x80;
				}
				break;
				
		default:
		break;
	}
}

unsigned char course = 0x00; //1 = upright, 2 = upleft, 3 = downright, 4 = downleft
volatile unsigned char ballC = 0x00;
volatile unsigned char ballR = 0x00;
enum states1{init1,move,upRight, upLeft, downRight, downLeft, WTF} state1;

void TickBall(){
	switch(state1){
		case init1:
			state1 = move;
			break;
			
		case move:
			//bouncing
			if((course == 0x01) && (ballR == 0x01)){
				course = 0x03;
			}
			else if((course == 0x03) && (ballC == 0x7F)){
				course = 0x04;
			}
			else if((course == 0x04) && (ballR == 0x80)){
				course = 0x02;
			}
			else if((course == 0x02) && (ballC == 0xFE)){
				course = 0x01;
			}
			
			
			
			
			//direction
			if((course) == 0x01){
				state1 = upRight;
			}
			else if((course) == 0x02){
				state1 = upLeft;
			}
			else if((course) == 0x03){
				state1 = downRight;
			}
			else if((course) == 0x04){
				state1 = downLeft;
			}
			else if((course) == 0x05){
				state1 = WTF;
			}
			else{
				state1 = move;
			}
			break;
			
		case upRight:
			state1 = move;
			break;
			
		case upLeft:
			state1 = move;
			break;
			
		case downRight:
			state1 = move;
			break;
			
		case downLeft:
			state1 = move;
			break;
			
		case WTF:
			state1 = move;
			
		default:
			state1 = move;
			break;		
	}
	switch(state1){
		case init1:
		break;
		
		case move:
		break;
		
		case upRight:
				ballC = (ballC << 1);
				ballC = ballC + 1;
			
				ballR = (ballR >> 1);
				break;
			
		case upLeft:
			ballC = (ballC >> 1);
			ballC = ballC - 0x80;
			
			ballR = (ballR >> 1);
			
	
		
		break;
		
		case downRight:
			ballC = (ballC << 1);
			ballC = ballC + 1;
			
			ballR = (ballR << 1);
			
			
		break;
		
		case downLeft:
			ballC = (ballC >> 1);
			ballC = ballC - 0x80;
			
			ballR = (ballR << 1);
			
		
		
		break;
		
		case WTF:
			ballC == 0x00;
					ballR == 0xFF;
		break;
	}
		
		
}



int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0x00; PORTB = 0xFF;
    DDRA = 0xFF; PORTA = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    TimerSet(100);
	TimerOn();

/*
	PaddleC = ~(0x18);
	PaddleR = 0x80;
	state = init;
	*/
	
	ballR = 0x10;
	ballC = ~(0x04);
	course = 0x01;	
	state1 = init1;

    /* Insert your solution below */
    while (1) {
		//TickPaddle();
		TickBall();
		//PORTC = PaddleC;
		//PORTD = PaddleR;
		PORTC = ballC;
		PORTD = ballR;
		while(!TimerFlag);
		TimerFlag = 0;
      
		
		
   }
    return 1;
}

	




	
