#include <avr/io.h>
#include <util/delay.h>
#define HC595_PORT   PORTB
#define HC595_DDR    DDRB
#define HC595_DS_POS PB0      //Data pin (DS) pin location
#define HC595_SH_CP_POS PB1      //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS PB2      //Store Clock (ST_CP) pin location

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
   for(uint8_t i=0;i<8;i++)
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
   for(uint8_t i=0;i<30;i++)
   {
      _delay_loop_2(0);
   }
}


#define HC595_PORT1   PORTB
#define HC595_DDR1    DDRB
#define HC595_DS_POS1 PB3      //Data pin (DS) pin location
#define HC595_SH_CP_POS1 PB4      //Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS1 PB5      //Store Clock (ST_CP) pin location

void HC595Init1()
{HC595_DDR1|=((1<<HC595_SH_CP_POS1)|(1<<HC595_ST_CP_POS1)|(1<<HC595_DS_POS1));}

#define HC595DataHigh1() (HC595_PORT1|=(1<<HC595_DS_POS1))
#define HC595DataLow1() (HC595_PORT1&=(~(1<<HC595_DS_POS1)))

void HC595Pulse1()
{
HC595_PORT1|=(1<<HC595_SH_CP_POS1);//HIGH
HC595_PORT1&=(~(1<<HC595_SH_CP_POS1));//LOW
}

void HC595Latch1()
{
HC595_PORT1|=(1<<HC595_ST_CP_POS1);//HIGH
_delay_loop_1(1);
HC595_PORT1&=(~(1<<HC595_ST_CP_POS1));//LOW
_delay_loop_1(1);
}

void HC595Write1(uint8_t data1)
{
for(uint8_t i=0;i<8;i++)
{
if(data1 & 0b10000000)
{
HC595DataHigh1();
}
else
{
HC595DataLow1();
}

HC595Pulse1();
data1=data1<<1;

}
HC595Latch1();
}

void Wait1()
{
for(uint8_t i=0;i<30;i++)
{
_delay_loop_2(0);
}
}

