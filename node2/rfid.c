#include<lpc21xx.h>
void UART_init(void);
unsigned char UART_RX(void);
void UART_STR(char *p);
void UART_Tx(unsigned char d);
int main()
{
    int i=0;
	unsigned char rxbyte[12];
	UART_init();
	while(1)
	{
	  i=0;
	  while(i<12)
	  {
	  rxbyte[i]=UART_RX();
	  i++;
	  }
	  UART_STR(rxbyte);

	  
	}
}
void UART_init()
{
	  PINSEL0 |= 0x05;
		U0LCR=0x83;
		U0DLL=97;
		U0DLM=0;
	  U0LCR=0x03;
}
void UART_Tx(unsigned char d)
{
  while(((U0LSR>>5)&1)==0);
     U0THR=d;	
}
void UART_STR(char *p)
{
	while(*p!='\0')
	{
		UART_Tx(*p++);
	}
}
unsigned char UART_RX()
{
	while((U0LSR&1)==0);
	return U0RBR;
}
