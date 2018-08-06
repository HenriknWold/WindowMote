#define F_CPU	8000000UL
#define BUAD	9600
#define BRC		((F_CPU/16UL/BUAD)-1)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FSM_TEST.h"

#define LCD_RST_set  PORTH |=  (1<<0)    //external reset input
#define LCD_RST_clr  PORTH &=~ (1<<0)

#define LCD_DC_set   PORTH |=  (1<<1)    //data/command
#define LCD_DC_clr   PORTH &=~ (1<<1)

#define SDIN_set     PORTH |=  (1<<2)    //serial data input
#define SDIN_clr     PORTH &=~ (1<<2)

#define SCLK_set     PORTH |=  (1<<3)    //serial clock input
#define SCLK_clr     PORTH &= ~(1<<3)


#include "nokia5110_chars.h"
#include "GE.h"
void LCD_write_byte(unsigned char dat, unsigned char command);
void LCD_init();
void LCD_clear();
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_write_char(unsigned char c);
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void LCD_State(TSystem *pSystem,int nr);

#define RX_BUFFER_SIZE 128
char RxBuffer[RX_BUFFER_SIZE];

#define NUM_SYSTEMS 3
TSystem rgSystems[NUM_SYSTEMS];


#define MAX_STR_LEN 20
char InputStr[MAX_STR_LEN];
char input_char;
int  input_str_len=0;

volatile uint8_t rxReadpos=0;
volatile uint8_t rxWritepos=0;
volatile int teller=1;

int USARTHasData();

void RXTX_Setup(void);
void USARTWriteChar(char data);
void Print_State(char *InputStr);
char getChar(void);
void ungetChar(void);
void clearString(char *str, int len);
void Set_input(char *InputStr);
void Get_Input(char *InputStr);

void Eject_System(char *InputStr);
void Inject_System(char *InputStr);
void printUARTstring(char *str)
{
	int i;
	for(i=0; i<strlen(str); i++){
		USARTWriteChar(str[i]);
	}
	return;
}

void Setup_timer(void);


volatile char dutyCycle=0;
void Find_State(TSystem *pSystem);

void setMotorParamSfwd(TSystem *pSystem, int nValue, int nr);
void setMotorParamSrev(TSystem *pSystem, int nValue, int nr);
void setMotorParamTrev(TSystem *pSystem, int nValue, int nr);
void setMotorParamTfwd(TSystem *pSystem, int nValue, int nr);

//initialiserer fart og tid
void initMotors(TSystem *pSystem)
{
	pSystem->SFWD = 50;
	pSystem->SREV = 50;
	pSystem->TFWD = 3000;
	pSystem->TREV = 3000;
	pSystem->current_state=EJECTED;
	pSystem->n = 0;
}

//int NEXT_STATE = EJECTED; // Must be global in scope

/* Called every tie the timer ISR fires (1msec default) */
void pollState(TSystem *pSystem)
{
	switch(pSystem->current_state)
	{
		case EJECTED:
		
		if(pSystem->next_state == INSERTING)
		{
			pSystem->nTimeFwdLeft = 10*(pSystem->TFWD);
			
			pSystem->current_state= INSERTING;
		  
			printUARTstring("200 OK\r\n");
			pSystem->next_state= '\0';
			
		}
		if(pSystem->next_state == EJECTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}
		break;
		case INSERTING:
		pSystem->nTimeFwdLeft--;
		
		if(pSystem->nTimeFwdLeft<= 0)
		{
			pSystem->current_state = INSERTED;
		}
		if(pSystem->next_state == EJECTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}
		if(pSystem->next_state == INSERTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}
		
		break;
		case INSERTED:
		if(pSystem->next_state == EJECTING){
			
			pSystem->nTimeRevLeft = 10*(pSystem->TREV);
			pSystem->current_state= EJECTING;
			printUARTstring("200 OK\r\n");
			pSystem->next_state= '\0';
		}
		if(pSystem->next_state == INSERTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}
		break;
		
		case EJECTING:
		pSystem->nTimeRevLeft--;
		if(pSystem->nTimeRevLeft<= 0)
		{
			pSystem->current_state = EJECTED;
			
		}
		if(pSystem->next_state == EJECTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}
		if(pSystem->next_state == INSERTING)
		{
			printUARTstring("400 ERROR\r\n");
			pSystem->next_state= '\0';
		}break;
		
		
	}
}

int main(void)
{
	DDRB= 0xff;
	DDRA=0x00;
	DDRH = 0x0F;
	PORTB=0x00;
	DDRC=0xff;
	PORTC=0xff;
	
	LCD_init();
	RXTX_Setup();
	Setup_timer();
	sei();
	
	int i;
	for(i=0;i<NUM_SYSTEMS;i++)
	{
		initMotors(&rgSystems[i]);
	}
	clearString(InputStr, sizeof(InputStr));
	
	
	while (1)
	{
		if(USARTHasData())
		{
			input_char = toupper(getChar());
			USARTWriteChar(input_char); // echo back
			if(input_char == '\r')
			{
				printUARTstring("\r\n");
				if ((strncmp(InputStr,"GET", 3) == 0)||(strncmp(InputStr,"STATE", 5) == 0)||(strncmp(InputStr,"EJECT", 5) == 0)||(strncmp(InputStr,"SET",3) == 0)||(strncmp(InputStr,"INJECT",6) == 0))
				{
					
					if (strncmp(InputStr,"SET",3) == 0)
					{
						Set_input(InputStr);
					}
					if ((strncmp(InputStr,"INJECT",6) == 0) && (strlen(InputStr)<=8))
					{
						Inject_System(InputStr);
					}
					if ((strncmp(InputStr,"EJECT", 5) == 0) && (strlen(InputStr)<=7))
					{
						Eject_System(InputStr);
					}
					if ((strncmp(InputStr,"STATE", 5) == 0)&& (strlen(InputStr)<=7))
					{
						Print_State(InputStr);
					}
					if (strlen(InputStr)<6)
					{
						printUARTstring("400 ERROR\r\n");
					}
					if ((strncmp(InputStr,"GET", 3) == 0))
					{
						Get_Input(InputStr);
						clearString(InputStr, sizeof(InputStr));
						input_str_len=0;
						input_char=0;//nytt
						InputStr[input_str_len--] = input_char;
					}
					else
					{
						clearString(InputStr, sizeof(InputStr));
						input_str_len=0;
						input_char=0;//nytt
						InputStr[input_str_len--] = input_char;
					}
					
				}
				else
				{
					printUARTstring("400 ERROR\r\n");
					clearString(InputStr, sizeof(InputStr));
					input_str_len=0;
					input_char=0;//nytt
					InputStr[input_str_len--] = input_char;//nytt
				}
			}
			if (input_char == '\b'||input_char == '|')
			{
				if (input_char == '\b')
				{
					input_char=0;
					InputStr[input_str_len--] = 0;
					
					USARTWriteChar(' '); // echo back
					USARTWriteChar('\b'); // echo back
					if(input_str_len <= 0)
					{
						input_str_len=0;
					}
					
				}

				if (input_char == '|')
				{
					
					for (int s=0;s<=input_str_len;s++)
					{
						USARTWriteChar('\b');
						USARTWriteChar(' ');
						USARTWriteChar('\b'); // echo back
						
					}
					clearString(InputStr, sizeof(InputStr));
					input_str_len=0;
				}
				
			}
			else if(input_char != ('\b'||'|' ||'\r'))
			{
				InputStr[input_str_len++] = input_char;
				
			}
			
		}
		if (!(PINA & 0x20))
		{
			teller++;
			_delay_ms(200);
			if (teller>=4)
			{
				teller=1;
			}
			
		}
		PORTC=~(1<<(teller-1) );
		for(int s=0;s<3;s++)
		LCD_State(&rgSystems[s],s);
	}
}

void Get_Input(char *InputStr)
{
	char fart[10];
	char tid[10];
	char para[4];
	int nr;
	
	sscanf(InputStr,"%*[^0123456789]%d",&nr);
	int TFWD= rgSystems[nr-1].TFWD;
	int TREV= rgSystems[nr-1].TREV;
	int SFWD= rgSystems[nr-1].SFWD;
	int SREV= rgSystems[nr-1].SREV;
	sscanf(InputStr,"%*s %[^=]=",para);
	
	if ((strcmp(para,"TFWD") == 0)||(strcmp(para,"TREV") == 0)||(strcmp(para,"SREV") == 0)||(strcmp(para,"SFWD") == 0))
	{
		if (strcmp(para,"TFWD") == 0)
		{
			printUARTstring("200 OK\r\n");
			itoa(TFWD,tid,10);
			printUARTstring("TFWD=");
			printUARTstring(tid);
			printUARTstring("\r\n");
		}
		
		if (strcmp(para,"TREV") == 0)
		{
			printUARTstring("200 OK\r\n");
			itoa(TREV,tid,10);
			printUARTstring("TREV=");
			printUARTstring(tid);
			printUARTstring("\r\n");
		}
		if (strcmp(para,"SFWD") == 0)
		{
			printUARTstring("200 OK\r\n");
			itoa(SFWD,fart,10);
			printUARTstring("SFWD=");
			printUARTstring(fart);
			printUARTstring("\r\n");
		}
		if (strcmp(para,"SREV") == 0)
		{		printUARTstring("200 OK\r\n");
			itoa(SREV,fart,10);
			printUARTstring("SREV=");
			printUARTstring(fart);
			printUARTstring("\r\n");
			
		}
		else
		{
			clearString(InputStr, sizeof(InputStr));
			input_str_len=0;
		}
	}
	else
	{
		printUARTstring("400 ERROR\r\n");
		clearString(InputStr, sizeof(InputStr));
		input_str_len=0;
	}
}
void Set_input(char *InputStr)
{
	int tid;
	int fart;
	int nr;
	char para[4];
	sscanf(InputStr,"%*[^0123456789]%d",&nr);
	sscanf(InputStr,"%*s %[^=]=",para);
	
	if ((strcmp(para,"TFWD") == 0)||(strcmp(para,"TREV") == 0)||(strcmp(para,"SREV") == 0)||(strcmp(para,"SFWD") == 0))
	{
		if ((strcmp(para,"TFWD") == 0))
		{
			sscanf(InputStr,"%*s %*[^0123456789]%d",&tid);
			setMotorParamTfwd(&rgSystems[nr-1], tid, (((nr-1)*2)+1));
			
		}
		if ((strcmp(para,"TREV") == 0))
		{
			sscanf(InputStr,"%*s %*[^0123456789]%d",&tid);
			setMotorParamTrev(&rgSystems[nr-1], tid, (((nr-1)*2)+1));
		}
		if ((strcmp(para,"SFWD") == 0))
		{
			sscanf(InputStr,"%*s %*[^0123456789]%d",&fart);
			setMotorParamSfwd(&rgSystems[nr-1], fart, (((nr-1)*2)+1));
		}
		if ((strcmp(para,"SREV") == 0))
		{
			sscanf(InputStr,"%*s %*[^0123456789]%d",&fart);
			setMotorParamSrev(&rgSystems[nr-1], fart, (((nr-1)*2)+1));
		}
		
	}
	else
	{
		printUARTstring("400 ERROR\r\n");
	}
}

//mye vittig

void Eject_System(char *InputStr)
{
	int nr;
	sscanf(InputStr,"%*[^0123456789]%d",&nr);
	
	if (strncmp(InputStr,"EJECT",5) == 0)
	{
		rgSystems[nr-1].next_state = EJECTING;
		//LCD_write_english_string(15,(nr-1)," EJECTING");
	}
	if((nr<1)||(nr>3))
	{
		printUARTstring("400 ERROR\r\n");
	}
	
}
void Inject_System(char *InputStr)
{
	int nr;
	sscanf(InputStr,"%*[^0123456789]%d",&nr);
	
	if (strncmp(InputStr,"INJECT",6) == 0)
	{
		
		rgSystems[nr-1].next_state = INSERTING;
		//LCD_write_english_string(15,(nr-1)," INSERTING");
	}
	if((nr<1)||(nr>3))
	{
		printUARTstring("400 ERROR\r\n");
	}

	
}
void Print_State(char *InputStr)
{
	int nr;
	sscanf(InputStr,"%*[^0123456789]%d",&nr);
	
	if((nr<1)||(nr>3))
	{
		printUARTstring("400 ERROR\r\n");
		clearString(InputStr, sizeof(InputStr));
		input_str_len=0;
	}
	if (strncmp(InputStr,"STATE",5) == 0)
	{
		
		Find_State(&rgSystems[nr-1]);
	}
	
}
void Find_State(TSystem *pSystem)
{
	switch(pSystem->current_state)
	{
		case EJECTED:
		printUARTstring("200 OK\r\n");
		printUARTstring("EJECTED\r\n");
		break;
		
		case INSERTING:
		printUARTstring("200 OK\r\n");
		printUARTstring("INSERTING\r\n");
		break;
		
		case INSERTED:
		printUARTstring("200 OK\r\n");
		printUARTstring("INSERTED\r\n");
		break;
		case EJECTING:
		printUARTstring("200 OK\r\n");
		printUARTstring("EJECTING\r\n");
		break;
	}
	
}

void setMotorParamSfwd(TSystem *pSystem, int nValue, int nr)
{
	int SFWD_MIN = 0;
	int SFWD_MAX = 100;
	char SFWD[10];
	if((nValue >= SFWD_MIN)&&(nValue <= SFWD_MAX))
	{
		printUARTstring("200 OK\r\n");
		pSystem->SFWD = nValue;
		itoa(nValue,SFWD,10);
		LCD_write_english_string(42,nr,"   ");
		LCD_write_english_string(42,nr,SFWD);
	}
	else
	printUARTstring("400 ERROR\r\n");
}
void setMotorParamSrev(TSystem *pSystem, int nValue, int nr)
{
	int SREV_MIN = 0;
	int SREV_MAX = 100;
	char SREV[10];
	if((nValue >= SREV_MIN)&&(nValue <= SREV_MAX)){
		pSystem->SREV = nValue;
		printUARTstring("200 OK\r\n");
		itoa(nValue,SREV,10);
		LCD_write_english_string(65,nr,"   ");
		LCD_write_english_string(65,nr,SREV);
	}
	else
	printUARTstring("400 ERROR\r\n");
}
void setMotorParamTfwd(TSystem *pSystem, int nValue, int nr)
{
	int TFWD_MIN = 0;
	int TFWD_MAX = 3000;
	if((nValue >= TFWD_MIN)&&(nValue <= TFWD_MAX)){
		pSystem->TFWD = nValue;
		printUARTstring("200 OK\r\n");
	}
	else
	printUARTstring("400 ERROR\r\n");
}
void setMotorParamTrev(TSystem *pSystem, int nValue, int nr)
{
	int TREV_MIN = 0;
	int TREV_MAX = 3000;
	
	if((nValue >= TREV_MIN)&&(nValue <= TREV_MAX))
	{
		pSystem->TREV = nValue;
		printUARTstring("200 OK\r\n");
	}
	else
	printUARTstring("400 ERROR\r\n");
}

void clearString(char *str, int len)
{
	int i;
	for (i=0; i<len; i++)	{
		str[i] = 0;
	}
}
int USARTHasData()
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
void Setup_timer(void)//setter opp timer interrupt utifra databladet 1KHZ, 1MS
{
	TCCR0A=(1<<WGM01); // Velger CTC i Timer Counter Control Register A
	OCR0A=100;  // Velger Ticker verdi som skal sammenliknes (20HZ)
	TIMSK0=(1<<OCIE0A); // Velger at Interrupt Mask registert skal sammenlikne output med Match A( her velges evt overflow/
	TCCR0B= (1<<CS01);//||(1<<CS01); //velger prescaler til 256
}
void RXTX_Setup(void) // setter opp Reciving og transmitting utifra databladet
{
	UBRR0H = (BRC >>8);
	UBRR0L = BRC;
	
	UCSR0B= (1<<RXEN0)|(1<<RXCIE0)|(1<<TXEN0)/*|(1<<TXCIE0)*/; //enables interrupt og mottak  eneble transmitting og Complete Interrupt Enable
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // stop bit 8
}
ISR(USART0_RX_vect)// recieving interrupt
{
	RxBuffer[rxWritepos]= UDR0; // data kommer fra receiver
	
	rxWritepos++;
	
	if(rxWritepos>=RX_BUFFER_SIZE)
	{
		rxWritepos=0;
	}
	
}
ISR(TIMER0_COMPA_vect)//timer interrupt for fart
{
	
	int i;
	for(i=0;i<NUM_SYSTEMS;i++)
	{  
		pollState(&rgSystems[i]);
		
		if (!(PINA & 0x80)||!(PINA & 0x40)||(rgSystems[0].current_state == EJECTING)||(rgSystems[0].current_state==INSERTING)||(rgSystems[1].current_state == EJECTING)||(rgSystems[1].current_state==INSERTING)||(rgSystems[2].current_state == EJECTING)||(rgSystems[2].current_state==INSERTING))
		{
			
			if((rgSystems[0].current_state == EJECTING)||(rgSystems[0].current_state==INSERTING)||(rgSystems[1].current_state == EJECTING)||(rgSystems[1].current_state==INSERTING)||(rgSystems[2].current_state == EJECTING)||(rgSystems[2].current_state==INSERTING))
			{
				if((rgSystems[i].current_state==EJECTING))
				{

					
					PORTB |= (1<<(i+3));
					
					rgSystems[i].n++;
					if (rgSystems[i].n<=(rgSystems[i].SFWD)/5)// forward
					{
						PORTB &= ~ (1<<i);//PORTB &= ~(1<<i);
					}else
					{
						PORTB |=(1<<i);//PORTB |= (1<<i);
					}
					if(rgSystems[i].n>=20)
					{
						rgSystems[i].n=0;
					}
				

				}
				if((rgSystems[i].current_state == INSERTING))// revers
				{
					PORTB |=  (1<<i);
					
					rgSystems[i].n++;
					if (rgSystems[i].n<=(rgSystems[i].SREV)/5)
					{
						PORTB &= ~(1<<(i+3));//PORTB &= ~ (1<<i);
					}else
					{
						PORTB |= (1<<(i+3));//PORTB |= (1<<i);
					}
					if(rgSystems[i].n>=20)
					{
						rgSystems[i].n=0;
					}
				
					
				}
				
			}
			

			if (!(PINA & 0x80)||!(PINA & 0x40))
			{
				
				if (!(PINA & 0x80))
				{

					PORTB |= (1<<(teller-1));
					rgSystems[teller-1].n++;
					if (rgSystems[teller-1].n<=25)
					{
						PORTB  &= ~((1)<<(2+teller));//fungerer ikke
					}else
					{
						PORTB |= ((1)<<(2+teller));// fungerer ikke
					}
					if(rgSystems[teller-1].n>=50)
					{
						rgSystems[teller-1].n=0;
					}
				}
				if (!(PINA & 0x40))
				{

					PORTB |=  ((1)<<(2+teller));//fungerer ikke
					rgSystems[teller-1].n++;
					if (rgSystems[teller-1].n<=25)
					{
						PORTB &= ~(1<<(teller-1));//&= ~(teller-1);
					}else
					{
						PORTB |= (1<<(teller-1));
					}
					if(rgSystems[teller-1].n>=50)
					{
						rgSystems[teller-1].n=0;
					}
				}
				
			}
		}
		else if ((PINA & 0x80)||(PINA & 0x40)||(rgSystems[0].current_state != EJECTING)||(rgSystems[0].current_state!=INSERTING)||(rgSystems[1].current_state != EJECTING)||(rgSystems[1].current_state!=INSERTING)||(rgSystems[2].current_state != EJECTING)||(rgSystems[2].current_state!=INSERTING))
		{
			PORTB=0x00;
		}
		
		
	}
	
}

void LCD_write_byte(unsigned char dat, unsigned char command)
{
	unsigned char i;
	
	if (command == 1)
	LCD_DC_clr;
	else
	LCD_DC_set;

	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		SDIN_set;
		else
		SDIN_clr;
		SCLK_clr;
		dat = dat << 1;
		SCLK_set;
	}
}

void LCD_init()//initialiserer LCD-skjermen 
{
	LCD_RST_clr;
	_delay_us(1);
	LCD_RST_set;

	_delay_us(1);

	LCD_write_byte(0x21, 1);	// set LCD mode
	LCD_write_byte(0xB2, 1);	// set bias voltage
	LCD_write_byte(0x06, 1);	// temperature correction
	LCD_write_byte(0x13, 1);	// 1:48
	LCD_write_byte(0x20, 1);	// use bias command, vertical
	LCD_write_byte(0x0c, 1);	// set LCD mode,display normally
	LCD_clear();	                // clear the LCD
	
	for(int n = 0; n < 504; n++)
	{
		LCD_write_byte( GE[n], 0);
	}
	_delay_ms(3000);
	LCD_clear();
	
	
	LCD_write_english_string(0,0," M1=");
	LCD_write_english_string(0,2," M2=");
	LCD_write_english_string(0,4," M3=");
	LCD_write_english_string(0,1," SF!SR=");
	LCD_write_english_string(42,1,"50");
	LCD_write_english_string(60,1,"!");
	LCD_write_english_string(67,1,"50");
	LCD_write_english_string(0,3," SF!SR=");
	LCD_write_english_string(42,3,"50");
	LCD_write_english_string(60,3,"!");
	LCD_write_english_string(67,3,"50");
    LCD_write_english_string(0,5," SF!SR=");
	LCD_write_english_string(42,5,"50");
	LCD_write_english_string(60,5,"!");
	LCD_write_english_string(67,5,"50");
}

void LCD_clear()//Setter alle pixlene til 0
{
	unsigned int i;

	LCD_write_byte(0x0c, 1);// normal display
	LCD_write_byte(0x80, 1);

	for (i=0; i<504; i++)
	{
		LCD_write_byte(0, 0);
	}
}

void LCD_set_XY(unsigned char X, unsigned char Y)
{
	LCD_write_byte(0x40 | Y, 1);	// column
	LCD_write_byte(0x80 | X, 1);    // row
}

void LCD_write_char(unsigned char c)
{
	unsigned char line;

	c -= 32;

	for (line=0; line<6; line++)
	LCD_write_byte(font6x8[c][line], 0);
}

void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
{
	LCD_set_XY(X,Y);
	while (*s)
	{
		LCD_write_char(*s);
		s++;
	}
}

void LCD_State(TSystem *pSystem,int nr)
{
	switch(pSystem->current_state)
	{ 
		case EJECTED:
		LCD_write_english_string(25,(nr*2),"EJECTED  ");
		break;
		
		case INSERTING:
		LCD_write_english_string(25,(nr*2),"INSERTING");
		break;
		
		case INSERTED:
		LCD_write_english_string(25,(nr*2),"INSERTED  ");
		break;
		case EJECTING:
		LCD_write_english_string(25,(nr*2),"EJECTING ");
		break;
	}
	
}
