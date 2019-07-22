/*	Author: Moker(Ke) Bellomo
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Mini Project
 *	Exercise Description: Pong Game
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

unsigned char ballSpd = 500;
volatile char win = 0;

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
		PaddleC = ~(0x38);
		PaddleR = 0x80;
		break;
		
		case on:
		break;
		
		case right:
				if(PaddleC == 0x1F){
					break;
				}
				else{
					PaddleC = (PaddleC << 1);
					PaddleC = PaddleC + 1;
				}
		break;
		
		case left:
				if(PaddleC == 0xF8){
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

enum states2{init2,on2,right2,left2} state2;
volatile unsigned char Paddle2C = 0x00;
volatile unsigned char Paddle2R = 0x00;

void TickPaddle2(){
	switch(state2){
		case init2:
			state2 = on2;
			break;
			
		case on2:
		
			if((~PINB & 0x10) == 0x10){
				state2 = right2;
			}
			else if((~PINB & 0x08) == 0x08){
				state2 = left2;
			}
			else{
				state2 = on2;
			}
			break;
		
		case right2:
			state2 = on2;
			break;
		
		case left2:
			state2 = on2;
			break;
			
		default:
		state2 = init2;
		break;
	}
	
	switch(state2){
		case init2:
		Paddle2C = ~(0x38);
		Paddle2R = 0x01;
		break;
		
		case on2:
		break;
		
		case right2:
				if(Paddle2C == 0x1F){
					break;
				}
				else{
					Paddle2C = (Paddle2C << 1);
					Paddle2C = Paddle2C + 1;
				}
		break;
		
		case left2:
				if(Paddle2C == 0xF8){
					break;
				}
				else{
					Paddle2C = (Paddle2C >> 1);
					Paddle2C = Paddle2C - 0x80;
				}
				break;
				
		default:
		break;
	}
}

unsigned char course = 0x00; //1 = upright, 2 = upleft, 3 = downright, 4 = downleft
							//5 = down, 6 = up
volatile unsigned char ballC = 0x00;
volatile unsigned char ballR = 0x00;
unsigned char guess = 0x00;
unsigned char guess2 = 0x00;
enum states1{init1,move,upRight, upLeft, downRight, downLeft, down, up} state1;

void TickBall(){
	switch(state1){
		case init1:
			state1 = move;
			break;
			
		case move:
			//bouncing off wall
			if((~PINB & 0x20) == 0x20){ goto skip;} // turns off top wall bounce if player2
		
			//top wall;  
			if((course == 0x01) && (ballR == 0x01)){
				course = 0x03;
			}
			else if((course == 0x02) && (ballR == 0x01)){
				course = 0x04;
			}
			else if((course == 0x06) && (ballR == 0x01)){
				course = 0x05;
			}
			
			skip:
			// right wall
			if((course == 0x03) && (ballC == 0x7F)){ 
				course = 0x04;
			}
			else if((course == 0x01) && (ballC == 0x7F)){
				course = 0x02;
			}
			/*
			// bottom wall
			else if((course == 0x04) && (ballR == 0x80)){
				course = 0x02;
			}
			else if((course == 0x03) && (ballR == 0x80)){
				course = 0x01;
			}
			else if((course == 0x05) && (ballR = 0x80)){
				course 0x06;
			}
			*/
			// left wall
			else if((course == 0x02) && (ballC == 0xFE)){
				course = 0x01;
			}
			else if((course == 0x04) && (ballC == 0xFE)){
				course = 0x03;
			}
			
			//corner cases
			//top left
			if((ballC == 0xFE ) && (ballR == 0x01)){
				course = 0x03;
			}
			//bottom right
			else if((ballC == 0x7F) && (ballR == 0x80)){
				course = 0x02;
			}
			//top right
			else if((ballC == 0x7F ) && (ballR == 0x01)){
				course = 0x04;
			}
			//bottom left
			else if((ballC == 0xFE) && (ballR == 0x80)){
				course = 0x01;
			}
			
			//bouncing off paddle
			if((PaddleC | ballC) != 0xFF && (PaddleR == (ballR << 1))){
				guess = ballC;
				
				guess = (guess << 1);
				guess = guess + 1;
				
				guess2 = guess;
				
				guess2 = (guess2 << 1);
				guess2 = guess2 + 1;
				
				if(ballSpd > 50){
						ballSpd = ballSpd - 25;
				}
				
				if((~guess) > (~PaddleC)){  
					//bounce right
					course = 1;
				}
				else if(~(guess2) > (~PaddleC)){
					//bounce middle
					course = 6;
				}
				else{
					//bounce left
					course = 2;
				}
			} 
			// player1 lose condition counter
			else if((PaddleC | ballC) == 0xFF && (PaddleR == (ballR))){
				win = 1;
			}
			//if player 2 exists
			if((~PINB & 0x20) == 0x20){
				if((Paddle2C | ballC) != 0xFF && (Paddle2R == (ballR >> 1))){
					guess = ballC;
				
					guess = (guess << 1);
					guess = guess + 1;
					
					guess2 = guess;
				
					guess2 = (guess2 << 1);
					guess2 = guess2 + 1;
					
					if(ballSpd > 50){
						ballSpd = ballSpd - 25;
					}
					if((~guess) > (~Paddle2C)){
						course = 3;
					}
					else if(~(guess2) > (~Paddle2C)){
						course = 5;
					}
					else{
						course = 4;
					}
				}
				else if((Paddle2C | ballC) == 0xFF && (Paddle2R == (ballR))){
					win = 1;
				} 
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
				state1 = down;
			}
			else if((course) == 0x06){
				state1 = up;
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
			
		case down:
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
		
		case down:
			//ballC = ballC;
			ballR = (ballR << 1);
		break;
		
		case up:
			//ballC = ballC;
			ballR = (ballR >> 1);
		break;
	}
		
		
}





int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0x00; PORTB = 0xFF;
    DDRA = 0xFF; PORTA = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    unsigned char time = 1;
    unsigned char cnt = 0;
    unsigned char cnt2 = 0;
    unsigned char swap = 0;
    
    TimerSet(time);
	TimerOn();

	PaddleC = ~(0x38);
	PaddleR = 0x80;
	state = init;
	
	if((~PINB & 0x20) & 0x20){
		Paddle2C = ~(0x38);
		Paddle2R = 0x01;
		state = init2;
	}
	
	ballR = 0x80;
	ballC = ~(0x08);
	course = 0x05;	
	state1 = init1;
	

    /* Insert your solution below */
    while (1) {
		if(win == 1){
			PORTC = 0x00;
			PORTD = 0xFF;
			
		}
		if(cnt == 100){
			TickPaddle();
			
			if((~PINB & 0x20) & 0x20){
				TickPaddle2();
			}
			cnt = 0;
		}
		++cnt;
		if(cnt2 == ballSpd){
			TickBall();
			cnt2 = 0;
		}
		++cnt2;
		
		if(swap == 0){
			PORTC = PaddleC;
			PORTD = PaddleR;
			swap = 1;
		}
		else if(swap == 1){
			PORTC = Paddle2C;
			PORTD = Paddle2R;
			swap = 2;
		}
		else{
			PORTC = ballC;
			PORTD = ballR;
			swap = 0;
		}
		//reset
		if((~PINB & 0x04) == 0x04){
			win = 0;
			ballR = 0x02;
			ballC = ~(0x08);
			course = 0x05;	
			state1 = init1;
			ballSpd = 500;
		}
			
		while(!TimerFlag);
		TimerFlag = 0;	
   }
    return 1;
}

	




	
