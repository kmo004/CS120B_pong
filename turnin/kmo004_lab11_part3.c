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
#include <bit.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char GetKeypadKed(){
	PORTA = 0xEF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return('1'); }
	if(GetBit(PINA,1) == 0) {return('4'); }
	if(GetBit(PINA,2) == 0) {return('7'); }
	if(GetBit(PINA,3) == 0) {return('*'); }
	
	PORTA = 0xDF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return('2'); }
	if(GetBit(PINA,1) == 0) {return('5'); }
	if(GetBit(PINA,2) == 0) {return('8'); }
	if(GetBit(PINA,3) == 0) {return('0'); }
	
	PORTA = 0xBF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return('3'); }
	if(GetBit(PINA,1) == 0) {return('6'); }
	if(GetBit(PINA,2) == 0) {return('9'); }
	if(GetBit(PINA,3) == 0) {return('#'); }
	
	PORTA = 0x7F;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return('A'); }
	if(GetBit(PINA,1) == 0) {return('B'); }
	if(GetBit(PINA,2) == 0) {return('C'); }
	if(GetBit(PINA,3) == 0) {return('D'); }
	
	return('\0');
	
}

int main(void) {
    /* Insert DDR and PORT initializations */
    unsigned char x;
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0xF0; PORTA = 0x0F;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    LCD_init();

    /* Insert your solution below */
    while (1) {

		x = GetKeypadKed();
		LCD_Cursor(1);
		switch(x) {
			case '\0': PORTB = 0x1F; break;
			case '1': LCD_WriteData(1 + '0');break;
			case '2': LCD_WriteData(2 + '0');break;
			case '3': LCD_WriteData(3 + '0');break;
			case '4': LCD_WriteData(4 + '0');break;
			case '5': LCD_WriteData(5 + '0');break;
			case '6': LCD_WriteData(6 + '0');break;
			case '7': LCD_WriteData(7 + '0');break;
			case '8': LCD_WriteData(8 + '0');break;
			case '9': LCD_WriteData(9 + '0');break;
			case 'A': LCD_WriteData(17 + '0');break;
			case 'B': LCD_WriteData(18 + '0');break;
			case 'C': LCD_WriteData(19 + '0');break;
			case 'D': LCD_WriteData(20 + '0');break;
			case '*': LCD_WriteData(-6 + '0');break;
			case '0': LCD_WriteData(0 + '0');break;
			case '#': LCD_WriteData(-13 + '0');break;
			default: PORTB = 0x1B;	break;
			
			continue;
		}
   }
    return 1;
}

	
