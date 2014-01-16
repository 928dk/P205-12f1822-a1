/*
 * File:   main.c
 * Author: 928dk
 *
 * Created on September 23, 2012, 4:27 PM
 */
#include <pic12f1822.h>
//#include <pic.h>
//#include <delays.h> //for pic 18 delays (not pic 12/16)
#include <stdio.h> // for uart etc
#include <stdlib.h> //for strings etc
#include <htc.h>
//#include <GenericTypeDefs.h>
//#include <math.h>
//#include <signal.h>
//#include <stdarg.h>
//#include <string.h>
//#include <time.h>

//Leds
#define	TRIS_RED	TRISAbits.TRISA3
#define	TRIS_GREEN	TRISAbits.TRISA1
#define	TRIS_BLUE	TRISAbits.TRISA2

#define	PORT_RED	PORTAbits.RA3
#define	PORT_GREEN	PORTAbits.RA1
#define	PORT_BLUE	PORTAbits.RA2

//#####################################
typedef unsigned		u1;
//typedef unsigned short char     u4;     //nibble, semioctet
typedef unsigned char		u8;     //byte,octet
typedef unsigned short		u16;    //unsigned int,halfword,word,short
typedef unsigned short long	u24;
typedef unsigned long		u32;    //word,long
typedef unsigned long long      u64;    //long long, quad, quadword, int6
//typedef unsigned double double  u128;   //octaword, double quadword (nonstandart)

//typedef signed                  s1;
//typedef signed short char       s4;     //nibble, semioctet
typedef signed char		s8;     //byte,octet
typedef signed short		s16;    //signed int,halfword,word,short
typedef signed short long	s24;
typedef signed long		s32;    //word,long
typedef signed long long        s64;    //long long, quad, quadword, int6
//typedef signed double double    s128;   //octaword, double quadword (nonstandart)
/*

char                8-bit  (signed by default)
signed char         8-bit (signed)
unsigned char       8-bit (unsigned)
int                 16 bit (signed)
unsigned int        16-bit (unsigned)
short               Same as int
unsigned short      Same as unsigned int
short long          24-bit (signed)
unsigned short long 24-bit (unsigned)
long                32-bit (signed)
unsigned long       32-bit (unsigned)

 * Signed kan holde  + og - (eks -128 til 128)
 * Unsigned kan kun holde positive værdier (eks 0 til 255)

 */


// prototypes
void Init12f1822(void);
void PwrOnTest(u8 on,u8 off);
void AllLedsOn(void);
void AllLedsOff(void);
void InitTimer0(void);
void InitTimer1(void);
void InitInt(void);
void Init_IO(void);
void Init_AD(void);
void Init_PWM(void);
void Delayx(int y);
void Delay10x(int y);
void SoftwarePWM(int on,int off);

void led1on(void);
void led1off(void);
void led2on(void);
void led2off(void);
void led3on(void);
void led3off(void);
void led4on(void);
void led4off(void);
void led5on(void);
void led5off(void);
void led6on(void);
void led6off(void);

unsigned int a = 0;

void main(void)
{
unsigned int i= 0;
unsigned char tmp;
unsigned Trig = 1;

Init12f1822();
Init_IO();
Init_AD();
Init_PWM();

InitTimer1();
InitInt();

//INTCONbits.GIE = 1;	//enable global interrupts
//INTCONbits.PEIE = 1;	//perihperal interrupt enable
//PIE1bits.ADIE = 1;	//enable ADC interrupt

do
{
/*
ADCON0bits.ADGO = 1;     // start conversion, GO/DONE = 1 side 146 (16.2.6) A/D CONVERSION PROCEDURE
  while (ADCON0bits.ADGO); // vent
    a = (((unsigned int)ADRESH)<<8) | (ADRESL&0xFF);	//gem i a (8bits)
*/
//CCPR1L = ADRES/10; //write result of ad converter to pwm duty cycle (and devide by 10)

     if(Trig)
        {
            i++;
            if(i>1022)
            {
                Trig = 0;
            }
        }
        else
        {
            i--;
            if(!i)
            {
                Trig = 1;
            }
        }


    tmp = (unsigned char)(a&0x0003);
        CCP1CONbits.DC1B = tmp;
        tmp = (unsigned char)0x00FF&(a>>2);
        CCPR1L = tmp;
  

    }while (1);

}

void Init12f1822(void)
{
//OSCCON = 0b01111011 ; // INTERNAL OSC 16MHZ
/*a0 settings
OSCCONbits.SPLLEN = 1;	//Enable 4 x PLL to get 32Mhz clock
OSCCONbits.IRCF1 = 1;	//Set internal osc freq to 8Mhz HF
OSCCONbits.IRCF2 = 1;
OSCCONbits.IRCF3 = 1;
*/
OSCCON = 0b01110000; //a1 settings

WPUA = 8;

WDTCONbits.SWDTEN = 0; //software watchdog disable
WDTCONbits.WDTPS = 01011; //2 sekunder iflg manual

ANSELA  = 0;
//CM1CON0 = 0x00;   // Disable comparators disabled in a1
//ADCON1 = 0x0F;  // Disable Analog functions All pins as digital I/O

}

void Init_PWM()
{
/*http://www.micro-examples.com/public/microex-navig/doc/097-pwm-calculator.html
 * The standard PWM mode generates a Pulse-Width
 * modulation (PWM) signal on the CCP1 pin with up to 10
 * bits of resolution. The period, duty cycle, and resolution
 * are controlled by the following registers:
 * ? PR2 registers
 * ? T2CON registers
 * ? CCPR1L registers
 * ? CCP1CON registers
 *
1. Disable the CCP1 pin output driver by settingthe associated TRIS bit.
2. Load the PR2 register with the PWM period value.
3. Configure the CCP1 module for the PWM mode by loading the CCP1CON register with the appropriate values.
4. Load the CCPR1L register and the DC1B1 bits of the CCP1CON register, with the PWM duty cycle value.
5. Configure and start Timer2:
? Clear the TMR2IF interrupt flag bit of the PIR1 register. See Note below.
? Configure the T2CKPS bits of the T2CON register with the Timer prescale value.
? Enable the Timer by setting the TMR2ON bit of the T2CON register.
6. Enable PWM output pin:
? Wait until the Timer overflows and the TMR2IF bit of the PIR1 register is set. See Note below.
? Enable the CCP1 pin output driver by clearing the associated TRIS bit.
 */
//APFCONbits.CCP1SEL = 1; //CCP1-Pin -> RA5
//APFCONbits.CCP1SEL = 0; //CCP1-Pin -> RA2

//PR2 = 0b00001001;       // Period/frekvens 50khz
//T2CON = 0b00000111;     // Prescaler + Postscaler + on
//CCP1CON = 0x00111100;     // PWM Mode

/*a0 settings
 *
 * PR2 = 0b01111100 ;
T2CON = 0b00000101 ;
CCP1CON = 0b00001100 ;
CCPR1L = 0b00001100;     // Duty Cycle  50%
*/

//CCPR1L = 0b00000010;     // Duty Cycle  50%
//PIR1bits.TMR2IF=0;   // Interrupt
//T2CONbits.TMR2ON = 1; // Timer2 on



CCP1CON = 0b00001100;
CCPR1L = 0;
PR2 = 0xFF;
T2CON = 0b00000110;
}

void Init_AD(void)
{
//http://embedded-lab.com/blog/?p=3770

/*a0 settings
ADCON0bits.ADON = 0; //off to write to register

FVRCON = 0b11000010 ; // Configure FVR to 2.048 V for ADC //a0 setting

//ADCON1 = 0x0F;  // Disable Analog functions All pins as digital I/O
ADCON1bits.ADPREF0 = 1; // Vref+ is 2.048 V
ADCON1bits.ADPREF1 = 1; //ADCON1bits.ADPREF = 3

ADCON1bits.ADCS0 = 0; //ADCON1bits.ADCS   = 4 Fosc/4
ADCON1bits.ADCS1 = 0; //sample at TAD focs/4
ADCON1bits.ADCS2 = 0; //

ADCON1bits.ADFM = 1;	//right justified

ADCON0bits.CHS0 = 1; // an1 = 1 an0 = 0
ADCON0bits.CHS1 = 0;
ADCON0bits.CHS2 = 0; //
ADCON0bits.CHS3 = 0; //
ADCON0bits.CHS4 = 0; //




ADCON0bits.ADON = 1;  // enable A/D converter
*/


//TRISAbits.TRISA1 = 1;	//set RA1 ADC pin input
//ANSELAbits.ANSA1 = 1;	//set pin to analogue input
//a1 settings below

FVRCON = 0b10000010;
    while(!FVRCONbits.FVRRDY);

ADCON0 = 0b00001100;
ADCON1 = 0b10000011;
}

void Init_IO(void)
{
//Tris vælger om det er input(1) eller output (0)
//Med Port kan man skrive 0 eller 1 hvis pin er valgt som output
//hvis port er valgt som input kan man læse om der er 0 eller 1 på pin

TRISAbits.TRISA0 = 0; //icsp data in + led out
TRISAbits.TRISA1 = 1; //icsp clock in + analog input
TRISAbits.TRISA2 = 0; //led
TRISAbits.TRISA3 = 0; //mclr
TRISAbits.TRISA4 = 0;//led
TRISAbits.TRISA5 = 0;//PWM out

PORTA = 0 ;                     // clear porten
}

void PwrOnTest(u8 on,u8 off)
{

}

void AllLedsOn(void)
{


}

void AllLedsOff(void)
{

}

void InitTimer0(void)
{


}

void InitTimer1(void) //a1 settings
{
T1CON = 0b00110101;
TMR1H = 0;     // preset for timer1 MSB register
TMR1L = 0;     // preset for timer1 LSB register
}

void InitInt(void)
{

INTCON = 00000000;
PIE1bits.TMR1IE = 0;
PIR1bits.TMR1IF = 0;
}

void led1on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 0; //set as output to enable control of leds
TRISAbits.TRISA2 = 0; //set as output to enable control of leds
TRISAbits.TRISA4 = 1;
PORTAbits.RA0 = 0;
PORTAbits.RA2 = 1;
PORTAbits.RA4 = 1;

}

void led1off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA4 = 1;
PORTAbits.RA0 = 0;
PORTAbits.RA2 = 0;
PORTAbits.RA4 = 0;

}

void led2on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 0; //set as output to enable control of leds
TRISAbits.TRISA2 = 0; //set as output to enable control of leds
TRISAbits.TRISA4 = 1;
PORTAbits.RA0 = 1;
PORTAbits.RA2 = 0;
PORTAbits.RA4 = 0;
}

void led2off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA4 = 1;
PORTAbits.RA0 = 0;
PORTAbits.RA2 = 0;
PORTAbits.RA4 = 0;

}

void led3on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA4 = 0; //set as output to enable control of leds
TRISAbits.TRISA2 = 0; //set as output to enable control of leds
TRISAbits.TRISA0 = 1;
PORTAbits.RA4 = 0;
PORTAbits.RA2 = 1;
PORTAbits.RA0 = 1;
}

void led3off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA4 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA0 = 1;
PORTAbits.RA4 = 0;
PORTAbits.RA2 = 0;
PORTAbits.RA0 = 0;
}

void led4on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 1;
TRISAbits.TRISA2 = 0; //set as output to enable control of leds
TRISAbits.TRISA4 = 0; //set as output to enable control of leds
PORTAbits.RA4 = 1;
PORTAbits.RA2 = 0;
PORTAbits.RA0 = 0;
}

void led4off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA4 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA0 = 1;
PORTAbits.RA4 = 0;
PORTAbits.RA2 = 0;
PORTAbits.RA0 = 0;
}

void led5on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 0;//set as output to enable control of leds
TRISAbits.TRISA2 = 1;
TRISAbits.TRISA4 = 0; //set as output to enable control of leds
//PORTAbits.RA2 = 0;
PORTAbits.RA0 = 1;
PORTAbits.RA4 = 0;
}

void led5off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA4 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA0 = 1;
PORTAbits.RA0 = 0;
//PORTAbits.RA2 = 0;
PORTAbits.RA4 = 0;
}

void led6on(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA0 = 0;//set as output to enable control of leds
TRISAbits.TRISA2 = 1;
TRISAbits.TRISA4 = 0; //set as output to enable control of leds
PORTAbits.RA4 = 1;
//PORTAbits.RA2 = 0;
PORTAbits.RA0 = 0;
}

void led6off(){
    //TRISAbits.TRISA0 = 1; //set as input to go tristate (high impedance)
TRISAbits.TRISA4 = 1; //set as output to enable control of leds
TRISAbits.TRISA2 = 1; //set as output to enable control of leds
TRISAbits.TRISA0 = 1;
PORTAbits.RA0 = 0;
//PORTAbits.RA2 = 0;
PORTAbits.RA4 = 0;
}

void Delayx(int y){
    int x=0;
        for (x=0; x<y; x=x+1) {
            asm("nop ");
     }
}

void Delay10x(int y){
    int x=0;
        for (x=0; x<y; x=x+1) {
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
            asm("nop ");
    }
}
/*
 *
 */
void SoftwarePWM(int on,int off)
{
    led1on();
    Delayx(on);
    led1off();
    Delayx(off);
    led2on();
    Delayx(on);
    led2off();
    Delayx(off);
    led3on();
    Delayx(on);
    led3off();
    Delayx(off);
    led4on();
    Delayx(on);
    led4off();
    Delayx(off);
    led5on();
    Delayx(on);
    led5off();
    Delayx(off);
    led6on();
    Delayx(on);
    led6off();
    Delayx(off);
}


