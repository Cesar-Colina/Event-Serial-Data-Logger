/* 
 * File:   EventDataLogger.c
 * Author: cesar colina
 * 
 * Created on November 13, 2021, 8:17 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


/*
 * 
 */
//values of the clock
uint8_t tenths = 0;
uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
uint16_t days = 0;

//character arrays for the clock values that will be outputted
char Tenths[] = "00";
char Seconds[] = "00";
char Minutes[] = "00";
char Hours[] = "00";
char Days[] = "000";

// channel labels
char C1[] = "Channel 1 ";
char C2[] = "Channel 2 ";
char C3[] = "Channel 3 ";
char C4[] = "Channel 4 ";
uint8_t size = sizeof (C1); // size is the same

// name and ID
char name[] = "Cesar Colina\n\r";
char ID[] = "1601637\n\r";




void Clock_init(){
    TCCR0A |= (1<<1); // Set it in CTC
    TIMSK |= (1<<0); //Timer/Counter0 Output Compare Match A Interrupt Enable
    OCR0A = 179; // calculated value for 0.1 seconds
    sei(); // enable interrupts
    TCCR0B |= ((1<<2) | (1<<0)); // 1024 prescaler
    
}
    
void Uart_init() {
    //default USCRA works
    UCSRB |= (1 << 3); // transmitter enable
    UCSRC |= (1 << 2) | (1 << 1); // 8 bit character size
    UBRRL = 2; // U2X disabled and baudrate is 38.4k


}

void Button_Init() {
    DDRD &= ~(1 << DDD6); //switch input button 1
    PORTD |= (1 << PORTD6); //pullup on

    DDRD &= ~(1 << DDD5); // switch input button 2
    PORTD |= (1 << PORTD5);

    DDRD &= ~(1 << DDD4); // switch input button 3
    PORTD |= (1 << PORTD4);

    DDRD &= ~(1 << DDD3); // switch input button 4
    PORTD |= (1 << PORTD3);

}

void USART_Transmit(char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSRA & (1 << UDRE)))
        ;
    /* Put data into buffer, sends the data */
    UDR = data;
}

void USART_nameid(){
    // output name
    for (uint8_t i = 0; i < sizeof(name); i++)
    {
        USART_Transmit(name[i]);
    }
    // output ID
    for (uint8_t i = 0; i < sizeof(ID); i++)
    {
        USART_Transmit(ID[i]);
    }
}

void Button_Check(){

    //if button 1 is pushed 
    if(!(PIND & (1<<PIND6))){
        //wait for user to let go
        while(!(PIND & (1<<PIND6))){}
        output_time();
        USART_Transmit(' ');
        for (uint8_t i = 0; i < size; i++) {
            USART_Transmit(C1[i]);
        }
         // newline and spacing
        USART_Transmit('\n');
        USART_Transmit('\r');

    }
    
    //if button 2 is pushed 
    if(!(PIND & (1<<PIND5))){
        //wait for user to let go
        while(!(PIND & (1<<PIND5))){}
        output_time();
        USART_Transmit(' ');
        for (uint8_t i = 0; i < size; i++) {
            USART_Transmit(C2[i]);
        }
         // newline and spacing
        USART_Transmit('\n');
        USART_Transmit('\r');
    }
    
    //if button 3 is pushed 
    if(!(PIND & (1<<PIND4))){
        //wait for user to let go
        while(!(PIND & (1<<PIND4))){}
        output_time();
        USART_Transmit(' ');
        for (uint8_t i = 0; i < size; i++) {
            USART_Transmit(C3[i]);
        }
         // newline and spacing
        USART_Transmit('\n');
        USART_Transmit('\r');
    }
    
    //if button 4 is pushed 
    if(!(PIND & (1<<PIND3))){
        //wait for user to let go
        while(!(PIND & (1<<PIND3))){}
        output_time();
        USART_Transmit(' ');
        for (uint8_t i = 0; i < size; i++) {
            USART_Transmit(C4[i]);
        }
         // newline and spacing
        USART_Transmit('\n');
        USART_Transmit('\r');
        
    }
    
    
}

void output_time(){
    // convert the integer values to a character array using itoa
    itoa(tenths,Tenths,10);
    itoa(seconds,Seconds,10);
    itoa(minutes,Minutes,10);
    itoa(hours,Hours,10);
    itoa(days,Days,10);
    
    //output days
    for(uint8_t i = 0; i < sizeof(Days); i++)
    {
        USART_Transmit(Days[i]);
    }
    //output space
    USART_Transmit(' ');
    //output hours
    for(uint8_t i = 0; i < sizeof(Hours); i++)
    {
        USART_Transmit(Hours[i]);
    }
    //output :
    USART_Transmit(':');
    //output minutes
    for(uint8_t i = 0; i < sizeof(Minutes); i++)
    {
        USART_Transmit(Minutes[i]);
    }
    //output :
    USART_Transmit(':');
    //output seconds
    for(uint8_t i = 0; i < sizeof(Seconds); i++)
    {
        USART_Transmit(Seconds[i]);
    }
    // output .
    USART_Transmit('.');
    //output tenths
    for(uint8_t i = 0; i < sizeof(Tenths); i++)
    {
        USART_Transmit(Tenths[i]);
    }
   
}

int main(int argc, char** argv) {
    Clock_init();
    Button_Init();
    Uart_init();
    
    USART_nameid();
    while(1){
        Button_Check();
    }
   

    return (EXIT_SUCCESS);
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK){
    
   
    tenths++;
    //if a full second has occured
    if (tenths > 9)
    {
        tenths = 0;
        seconds++;
    }
    //if a minute has occurred
    if (seconds > 59)
    {
        seconds = 0;
        minutes++;
    }
    //if an hour has occurred
    if (minutes > 59)
    {
        minutes = 0;
        hours++;
    }
    //if a day has occurred
    if (hours > 23)
    {
        hours = 0;
        days++;
    }
    
    
}