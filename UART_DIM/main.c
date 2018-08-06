#define F_CPU	8000000UL
#define BUAD	9600
#define BRC		((F_CPU/16UL/BUAD)-1)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX_BUFFER_SIZE 128
char RxBuffer[RX_BUFFER_SIZE];

#define MAX_STR_LEN 20
char InputStr[MAX_STR_LEN];
char input_char;
int  input_str_len=0;

volatile uint8_t rxReadpos=0;
volatile uint8_t rxWritepos=0;

char peekChar(void);
char getChar(void);
char peekChar(void);
char Read_More(void);

void USARTWriteChar(char data);
int USARTHasData();
int RXTX_Setup(void);
int stop(void);
void Setup_timer(void);
int Button_Check(void);
void juster_styrke(int styrke);
volatile char n=0;
int lysstyrke=0;
 volatile char dutyCycle=0;
volatile char delta=200/32;


void clearString(char *str, int len)
{
	int i;
	for (i=0; i<len; i++)	{
		str[i] = 0;
	}
}

void printUARTstring(char *str)
{
	int i;
	for(i=0; i<strlen(str); i++){
		USARTWriteChar(str[i]);
	}
	return;
}


int main(void)//hovedprogram 
{
	int styrke;
	const char ch= '=';
	char *ret;
	
	
	DDRB= 0xff;
	PORTB=0xff;
	DDRA=0x00;
	

	Setup_timer();
	RXTX_Setup();
	sei();

	clearString(InputStr, sizeof(InputStr));
	printUARTstring("\r\nWelcome!!!\r\n");
	while (1)
	{
		if(USARTHasData())
		{
			
			input_char = getChar();
			USARTWriteChar(input_char); // echo back
			if(input_char == '\r'){
				printUARTstring("\r\n");
				if(strcmp(InputStr, "ned") == 0){
					dutyCycle=dutyCycle-delta;
					lysstyrke--;	
				}
				if(strcmp(InputStr, "opp") == 0){
					dutyCycle=dutyCycle+delta;
					lysstyrke++;
				}
			if(strcmp(InputStr,"lys=") >= 0 && strncmp(InputStr,"lys=",4) == 0){
				
		      if(strlen(InputStr)<=6){
					if(sscanf(InputStr,"%*[^0123456789]%d",&styrke)==1)
					{
						ret= strchr(InputStr,ch);
						juster_styrke(styrke);
					}
					else printUARTstring("Du tastet ikke inn et tall!\r\n");
				}else printUARTstring("Du tastet inn for mange siffer!\r\n");
			  
			}else printUARTstring("Prov igjen\r\n");
				clearString(InputStr, sizeof(InputStr));
				input_str_len=0;
			}else{
				InputStr[input_str_len++] = input_char;
			}
		}
		if(USARTHasData()==0)	
		{
			
				if( !(PINA & 0x40) )
				{	
					
					dutyCycle=dutyCycle-delta;
					lysstyrke--;
					stop();
				}
				if(!(PINA & 0x80))
				{
					
					dutyCycle= dutyCycle+delta;
					lysstyrke++;
					stop();
				}
			
		}
		
		
		if(lysstyrke<=0)
		{
			dutyCycle=0;
			lysstyrke=0;
		}
		
		if(lysstyrke>=32)
		{
			dutyCycle=200;
			lysstyrke=32;
		}
		
		
	}
#if 0		
				char c[32];
				int i=0;
				while ((rxReadpos != rxWritepos))
				{ 
					c[i] = getChar();
					//char f= getChar();
					USARTWriteChar(c[i]);
					i++;
					if(c[i]=='\0') break;
				}
				
				while((rxReadpos = rxWritepos))
				{
					
					if(c[i]=='o')
					{
						i++;
						
						if(c[i]=='p')
						{
							i++;
							if(c[i]=='p')
							{
								
								dutyCycle=dutyCycle+delta;
								lysstyrke++;
								c[i]='\0';
								stop();
								
								
							}
						}
						
					}
					if(c[i]=='n')
					{
						i++;
						
						if(c[i]=='e')
						{
							i++;
							if(c[i]=='d')
							{
								
								dutyCycle=dutyCycle-delta;
								lysstyrke--;
								c[i]='\0';
								stop();
						
								
							}
						}
						
					}
					i++;
					if(lysstyrke<=0)
					{
						dutyCycle=0;
						lysstyrke=0;
					}
					
					if(lysstyrke>=32)
					{
						dutyCycle=200;
						lysstyrke=32;
					}
				
			
		
			
		if(USARTHasData()==0)	
		{
			
				if( !(PINA & 0x40) )
				{	
					
					dutyCycle=dutyCycle-delta;
					lysstyrke--;
					stop();
				}
				if(!(PINA & 0x80))
				{
					
					dutyCycle= dutyCycle+delta;
					lysstyrke++;
					stop();
				}
			
		}
		
		
		if(lysstyrke<=0)
		{
			dutyCycle=0;
			lysstyrke=0;
		}
		
		if(lysstyrke>=32)
		{
			dutyCycle=200;
			lysstyrke=32;
		}
	

	}
			}
			
#endif
}
	
int USARTHasData()//skjekker om det blir tastet inn noe i terminalen
{
	if (rxReadpos != rxWritepos)
	  return 1;
	else
	  return 0;
}   

void USARTWriteChar(char data) 
{
	while (!(UCSR0A & (1 << UDRE0)))//fortsetter så lenge UDRE0 bit er på
	{}

	//Now write the data to USART buffer
	UDR0 = data; //transmitting data
	return;
	}
int RXTX_Setup(void) // setter opp Reciving og transmitting utifra databladet
{
	UBRR0H = (BRC >>8);
	UBRR0L = BRC;
	
	UCSR0B= (1<<RXEN0)|(1<<RXCIE0)|(1<<TXEN0)/*|(1<<TXCIE0)*/; //enables interrupt og mottak  eneble transmitting og Complete Interrupt Enable
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);// stop bit 8
}
void Setup_timer(void)//setter opp timer interrupt utifra databladet 
{
	TCCR0A=(1<<WGM01); // Velger CTC i Timer Counter Control Register A
	OCR0A= 3;  // Velger Ticker verdi som skal sammenliknes (20HZ)
	TIMSK0=(1<<OCIE0A); // Velger at Interrupt Mask registert skal sammenlikne output med Match A( her velges evt overflow/
	TCCR0B= (1<<CS01)|(1<<CS00); //velger prescaler til 64
	
}
int Button_Check(void)//skjekker om en knapp er trykket på eller ei
{
	if(PINA !=0xff)
	return 1;
	else
	return 0;
}
/*
char peekChar(void)// forskjellig fra getchar, ved a hva er den neste data som skal leses( sneek peek). kan brukes til string parsing
{
	char ret="\0";
	if (rxReadpos != rxWritepos)
	{
		ret=RxBuffer[rxReadpos];
	}
	
	return ret;
}
*/

 char getChar(void) //henter data som skal leses
{
	char ret='\0'; // setter til 0 char
	
	if (rxReadpos != rxWritepos) //hvis det skrives noe i terminalen( da er ikke write= read)
	{
		ret =RxBuffer[rxReadpos]; // ret blir endret til  rxbuffer plaseringen til readpos
		rxReadpos++; //increment neste input
		//rxReadpos= (rxReadpos + 1)% RX_BUFFER_SIZE;
		if(rxReadpos >= RX_BUFFER_SIZE)  // ringbuffer
		{
			rxReadpos=0;
			
			
		}
	}
	return ret;
}
int stop(void)//setter delay for debouncing
{
	_delay_ms(200);
}
void juster_styrke(int styrke)
{
	lysstyrke=styrke;
	dutyCycle= styrke*delta;
	
	
	
}

ISR(USART0_RX_vect)// recieving interrupt
{
	RxBuffer[rxWritepos]= UDR0; // data kommer fra receiver
	
	rxWritepos++;
	
	
	//rxWritepos= (rxWritepos + 1)% RX_BUFFER_SIZE;
	if(rxWritepos>=RX_BUFFER_SIZE)
	{
		rxWritepos=0;
		
	}
	//PORTB = (rxWritepos << 1);
}
ISR(TIMER0_COMPA_vect)//timer interrupt
{
	n=(n+1);
	if(n<=dutyCycle)
	{
		PORTB = (~lysstyrke <<  1) | (1-(1<<PB0));
	}
	else
	{
		PORTB =  ( ~lysstyrke << 1) | (1<<PB0);
	}
	if (n==200)
	{
		n=0;
	}
}
