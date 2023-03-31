/*
 * File:   main.c
 * Author: Edgar S
 *
 * Created on April 18, 2022, 5:47 PM
 */
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON      // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON     // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        
#define _XTAL_FREQ 4000000
#include <xc.h>
#include <pic.h>
#include <stdint.h>

uint8_t centena =0;
uint8_t decena =0;
uint8_t unidad =0;
uint8_t pot2 =0;
uint8_t flag_7seg =0;
uint8_t display[10];


void setup(void);
void reset_timer0(void);
void __interrupt()isr(void);
void divide(uint8_t *a, uint8_t *b,uint8_t *c);

void main(void) {
    setup();
    while(1){
        GO=1; // conversion del ADC
        divide(&centena,&decena,&unidad);
        __delay_us(10);
    switch(flag_7seg)  { // switch para 7 seg
        case 0:
            RE0=0;
            PORTD= display[decena];
            RE1=1;
            RE2=0;
            break;
        case 1:
            RE1=0;
            PORTD= display[unidad];
            RE2=1;
            RE0=0;
            break;
        case 2:
            RE2=0;
            PORTD= display[centena];
            RE0=1;
            RE1=0;
            break;  
        
    }
    }
}
void setup(){
  ANSEL=3; //ra0 y ra1 analogicos 
  ANSELH=0;
  TRISA=3; // inputs ra0 ra1
  TRISB=0;
  TRISC=0;
  TRISD=0;
  TRISE=0;
  
  ADCON1bits.ADFM=0;
  ADCON0= 0b01000001; // frecuencia de oscilacion , canal 0 , enable
  INTCONbits.GIE=1;
  INTCONbits.T0IF=0;
  INTCONbits.T0IE=1;
  INTCONbits.PEIE=1;
 // interrupciones  
 INTCONbits.T0IF=0;
 TMR0=8;
 OPTION_REGbits.T0CS=0;
 OPTION_REGbits.PS=0;
 OPTION_REGbits.PS0=0;
 OPTION_REGbits.PS1=1;
 OPTION_REGbits.PS2=1;
 // ini. puertos 
 PORTA=0;
 PORTB=0;
 PORTC=0;
 PORTD=0;
 PORTE=1;
 // display de 7 seg
    display[0]=0b11000000;
    display[1]=0b11111001;
    display[2]=0b10100100;
    display[3]=0b10110000;
    display[4]=0b10011001;
    display[5]=0b10001010;
    display[6]=0b10000010;
    display[7]=0b11111000;
    display[8]=0b10000000;
    display[9]=0b10011000;
}
void reset_timer0(void){
    INTCONbits.T0IF=0;
    TMR0 =8;
    OPTION_REGbits.T0CS=0;
    OPTION_REGbits.PS=0;
    OPTION_REGbits.PS0=0;
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS2=1;
}
void divide( uint8_t*a, uint8_t *b,uint8_t *c){
    *a= pot2/100;
    *b= (pot2-100*centena)/10;
    *c= pot2-100*centena-10*decena;
    
}
void __interrupt() isr(void){
   if (T0IF==1){
        reset_timer0();
        flag_7seg++;
        if(flag_7seg==3){
            flag_7seg=0;
        }
        T0IF=0;
   }
   if(ADIF==1){
       if(CHS0==0){
           PORTC = ADRESH;
           CHS0=1;
       }else if(CHS0==1){
           pot2=ADRESH;
           CHS0=0;
       }
       ADIF=0;
   }
}