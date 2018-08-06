#define F_CPU	8000000UL
#define BUAD	9600
#define BRC		((F_CPU/16UL/BUAD)-1)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadpos = 0;
uint8_t serialWritePos =0;

void appendSerial(char c);
void serialWrite(char c[]);

char RxBuffer[8];
uint8_t rxReadpos=0;
uint8_t rxWritepos=0;
char getChar(void);
//char peekChar(void);


int RXTX_Setup(void);

int main(void)
{
	

RXTX_Setup();
	
	sei();

	
	/* Replace with your application code */
	while (1)
	{
		char c= getChar();
		
		//serialWrite(c);
		USARTWriteChar(c);
		
		
		
	}
}

void USARTWriteChar(char data) {
	while (!(UCSR0A & (1 << UDRE0)))
	{ }

	//Now write the data to USART buffer
	UDR0 = data;
}
int RXTX_Setup(void)
{
	UBRR0H = (BRC >>8);
	UBRR0L = BRC;
	
	UCSR0B= (1<<RXEN0)|(1<<RXCIE0)|(1<<TXEN0);/*|(1<<TXCIE0);*/ //enables interrupt og mottak  eneble transmitting og Complete Interrupt Enable
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);// stop bit 8
}
/*
char peekChar(void)// hva er den neste data som skal leses( sneek peek)
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
	char ret= "\0";
	
	if (rxReadpos != rxWritepos)
	{
		ret = RxBuffer[rxReadpos];
		rxReadpos++;
		
		if(rxReadpos >= RX_BUFFER_SIZE)
		{
			rxReadpos=0;
			
			
		}
	}
	return ret;
}
/*
void appendSerial(char c)
{
	serialBuffer[serialWritePos] = c;
	serialWritePos++;
	
	if(serialWritePos>=TX_BUFFER_SIZE)
	{
		serialWritePos=0;
		
	}
	if(UCSR0A & (1<<UDRE0) ) //USART Data Register Empty
	{
		UDR0=0;
	}
	
}

void serialWrite(char c[])
{
	for(uint8_t i=0; i <strlen(c);i++)
	{
		appendSerial(c[i]);
	}
}
*/
ISR(USART0_TX_vect)
{
	if(serialReadpos!=serialWritePos)
	{
		UDR0= serialBuffer[serialReadpos];
		serialReadpos++;
		
		if(serialReadpos>= TX_BUFFER_SIZE)
		{
			serialReadpos=0;
			
		}
		
	}
}

ISR(USART0_RX_vect)
{
	RxBuffer[rxWritepos]= UDR0;
	
	rxWritepos++;
	
	if(rxWritepos>=RX_BUFFER_SIZE)
	{
		rxWritepos=0;
		
	}
}
