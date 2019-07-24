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

#include <avr/io.h>
#include <util/delay.h>
#define HC595_PORT   PORTA
#define HC595_DDR    DDRA
#define HC595_DS_POS PB0      //Data pin (DS) pin location
#define HC595_SH_CP_POS PB1      //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS PB2      //Store Clock (ST_CP) pin location
uint8_t i = 0;

void HC595Init()
{HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));}

#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))
#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))

void HC595Pulse()
{
   HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH
   HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW
}

void HC595Latch()
{
   HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
   _delay_loop_1(1);
   HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
   _delay_loop_1(1);
}

void HC595Write(uint8_t data)
{
   for(i=0;i<8;i++)
   {
      if(data & 0b10000000)
      {
         HC595DataHigh();
      }
      else
      {
         HC595DataLow();
      }

      HC595Pulse();
      data=data<<1; 

   }
   HC595Latch();
}

void Wait()
{
   for(i=0;i<30;i++)
   {
      _delay_loop_2(0);
   }
}



volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char ballSpd = 500;
unsigned char score = 0;
unsigned char score1 = 0;
unsigned char score2 = 0x08;

enum statesAI{initAI,onAI,rightAI,leftAI} stateAI;
volatile unsigned char PaddleAIC = 0x00;
volatile unsigned char PaddleAIR = 0x00;
void TickPaddleAI();

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
		
		case left2:
				if(Paddle2C == 0x1F){
					break;
				}
				else{
					Paddle2C = (Paddle2C << 1);
					Paddle2C = Paddle2C + 1;
				}
		break;
		
		case right2:
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
unsigned char guess3 = 0x00;
enum states1{init1,move,upRight, upLeft, downRight, downLeft, down, up} state1;

void TickBall(){
	switch(state1){
		case init1:
			state1 = move;
			break;
			
		case move:
			/*
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
			*/
			
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
			
			
			/*
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
			*/
			
			//paddle corner cases
			if(course == 3){
				guess = ballC;
				guess = (guess << 1) +1;
			}
			else if(course ==4){
				guess = ballC;
				guess = (guess >> 1);
				guess = guess - 0x80;
			}
			else{
				guess = ballC;
			}
			
			//bouncing off paddle
			if( ((PaddleC | guess) != 0xFF) && (PaddleR == (ballR << 1)) ){
				
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
					if(course == 3){
						course = 1;
					}
					else{
						course = 2;
					}
				}
				else{
					//bounce left
					course = 2;
				}
			}
			

			 
			// player1 lose condition counter
			else if(PaddleR == ballR){
				if(score2 == 0x00){
					score2 = 0x10;
				}
				else if(score2 == 0x10){
					score2 = 0x30;
				}
				else if(score2 == 0x30){
					score2 = 0x70;
				}
				else{
					score2 = score2;
				}
			}
			//if player 2 exists
			if((~PINB & 0x20) == 0x20){
				if(course == 1){
					guess = ballC;
					guess = (guess << 1) +1;
				}
				else if(course == 2){
					guess = ballC;
					guess = (guess >> 1);
					guess = guess - 0x80;
				}
				else{
					guess = ballC;
				}
				
				if( ((Paddle2C | guess) != 0xFF) && (Paddle2R == (ballR >> 1)) ){
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
						if(course == 1){
							course = 3;
						}
						else{
							course = 4;
						}
					}
					else{
						course = 4;
					}
				}
				//player2 lose coniditon
				else if(Paddle2R == ballR){
					if(score1 == 0x00){
						score1 = 0x01;
					}
					else if(score1 == 0x01){
						score1 = 0x03;
					}
					else if(score1 == 0x03){
						score1 = 0x07;
					}
					else{
						score1 = score1;
					}
				}
				else{
				}
			}
			else{
				if(course == 1){
					guess = ballC;
					guess = (guess << 1) +1;
				}
				else if(course == 2){
					guess = ballC;
					guess = (guess >> 1);
					guess = guess - 0x80;
				}
				else{
					guess = ballC;
				}
				if( ((PaddleAIC | guess) != 0xFF) && (PaddleAIR == (ballR >> 1)) ){
					
					guess = ballC;
					guess = (guess << 1);
					guess = guess + 1;
					
					guess2 = guess;
				
					guess2 = (guess2 << 1);
					guess2 = guess2 + 1;
					
					if(ballSpd > 50){
						ballSpd = ballSpd - 25;
					}
					if((~guess) > (~PaddleAIC)){
						course = 3;
					}
					else if(~(guess2) > (~PaddleAIC)){
						if(course == 1){
							course = 3;
						}
						else{
							course = 4;
						}
					}
					else{
						course = 4;
					}
				}
				//playerAI lose coniditon
				else if(PaddleAIR == ballR){
					if(score1 == 0x00){
						score1 = 0x01;
					}
					else if(score1 == 0x01){
						score1 = 0x03;
					}
					else if(score1 == 0x03){
						score1 = 0x07;
					}
					else{
						score1 = score1;
					}
				}
			}
			score = score1 | score2;
			
			//corner cases
			//bottom left
			if((ballC == 0xFE && ballR == 0x40) && ((PaddleC == 0xF8) || (PaddleC == 0xF1))){
				course = 1;
			}
			//bottom right 
			else if((ballC == 0x7F && ballR == 0x40) && ((PaddleC == 0x8F) || (PaddleC == 0x1F))){
				course = 2;
			}
			//top left
			else if((ballC == 0xFE && ballR == 0x02) && ( ((Paddle2C == 0xF8) || (Paddle2C == 0xF1)) || ((PaddleAIC == 0xF8) || (PaddleAIC == 0xF1)) )){
				course = 3;
			}
			//top right
			else if((ballC == 0x7F && ballR == 0x02) && ( ((Paddle2C == 0x8F) || (Paddle2C == 0x1F)) || ((PaddleAIC == 0xF8) || (PaddleAIC == 0xF1)) )){
				course = 4;
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



void TickPaddleAI(){
	switch(stateAI){
		case initAI:
			stateAI = onAI;
			break;
			
		case onAI:
		
			guess = ballC;
				
			guess = (guess << 1);
			guess = guess + 1;
				
			guess2 = guess;
				
			guess2 = (guess2 << 1);
			guess2 = guess2 + 1;
			
			if((~guess > ~PaddleAIC) ){
				stateAI = leftAI;
			}
			else if(~guess2 > ~PaddleAIC ){
				//middle
			}
			else if(ballC == 0xBF | ballC == 0x7F){
				// fixes overflow issue that makes it flicker back and forth
			}
			else{
					stateAI = rightAI;
				
			}
			break;
		
		case rightAI:
			stateAI = onAI;
			break;
		
		case leftAI:
			stateAI = onAI;
			break;
			
		default:
		stateAI = initAI;
		break;
	}
	
	switch(stateAI){
		case initAI:
		PaddleAIC = ~(0x38);
		PaddleAIR = 0x01;
		break;
		
		case onAI:
		break;
		
		case leftAI:
				if(PaddleAIC == 0x1F){
					break;
				}
				else{
					PaddleAIC = (PaddleAIC << 1);
					PaddleAIC = PaddleAIC + 1;
				}
		break;
		
		case rightAI:
				if(PaddleAIC == 0xF8){
					break;
				}
				else{
					PaddleAIC = (PaddleAIC >> 1);
					PaddleAIC = PaddleAIC - 0x80;
				}
				break;
				
		default:
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
    unsigned char cntPL = 0;
    unsigned char cntAI = 0;
    unsigned char cntBall = 0;
    unsigned char swap = 0;
    
    TimerSet(time);
	TimerOn();

	//player 1
	PaddleC = ~(0x38);
	PaddleR = 0x80;
	state = init;
	
	//player 2 or AI player
	if((~PINB & 0x20) & 0x20){
		Paddle2C = ~(0x38);
		Paddle2R = 0x01;
		state2 = init2;
	}
	else{
		PaddleAIC = ~(0x38);
		PaddleAIR = 0x01;
		stateAI = initAI;
	}
	
	/*
	//ball;
	ballR = 0x80;
	ballC = ~(0x08);
	course = 0x05;	
	state1 = init1;
	*/
	HC595Init();

    /* Insert your solution below */
    while (1) {
		HC595Write(score);
		
		if(cntPL == 50){
			TickPaddle();
			
			if((~PINB & 0x20) & 0x20){
				TickPaddle2();
			}
			cntPL = 0;
		}
		++cntPL;
		if(cntAI == 50){
				TickPaddleAI();
				cntAI = 0;
			}
		++cntAI;
		
		if(cntBall == ballSpd){
			TickBall();
			cntBall = 0;
		}
		++cntBall;
		
		if(swap == 0){
			PORTC = PaddleC;
			PORTD = PaddleR;
			swap = 1;
		}
		else if(swap == 1){
			if((~PINB & 0x20) & 0x20){
				PORTC = Paddle2C;
				PORTD = Paddle2R;
			}
			else{
				PORTC = PaddleAIC;
				PORTD = PaddleAIR;
			}
			swap = 2;
		}
		else{
			PORTC = ballC;
			PORTD = ballR;
			swap = 0;
		}
		//start ball
		if((~PINB & 0x04) == 0x04){
			//fixes giving point to player2/AI for lack of ball during startup
			if(score2 == 0x08){
				score2 = 0x00;
			}
			//score reset after 3 points by either player
			if((score1 == 0x07) | (score2 == 0x70)){
					score1 = score2 = score = 0x00 ;
					
			}
			ballR = 0x04;
			ballC = ~(0x08);
			course = 0x05;	
			state1 = init1;
			ballSpd = 500;
			
			
		}
		
		if((~PINB & 0x40) == 0x40){
			score2 = score1 = score = 0x00;
			ballR = 0x00;
			ballC = 0xFF;
		}
			
			
		while(!TimerFlag);
		TimerFlag = 0;	
   }
    return 1;
}

	




	
