#include<LPC21xx.h>
#include<string.h>
typedef unsigned int u32;

#define LED1 17
#define LED2 18
#define token 3
#define RFmsgid 0x101
typedef struct 
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 databyteA;
	u32 databyteB;
}CAN2_MSG;

void can2_init(void);
void can2_tx(CAN2_MSG m1);
void can2_rx(CAN2_MSG *m1);
void delay_ms(int);
void UART_init(void);
unsigned char UART_RX(void);

int main()
{
	CAN2_MSG m1;
	int i=0,j,count=0;
	unsigned char p[token][13]={"0600670ECFA0","060067D961D9","0600676AC2C9"};
	unsigned char rxbyte[13];
	IODIR0 |= 1<<LED1|1<<LED2;
	//IOSET0=1<<LED1|1<<LED2;
	UART_init();
	can2_init();
	m1.id=RFmsgid;
	m1.rtr=0;//data frame
	m1.dlc=4;
	while(1)
	{
		IOCLR0=1<<LED1|1<<LED2;
		i=0;count=0;
		for(i=0;i<12;i++)
	               {
	                 rxbyte[i]=UART_RX();
	               }
		rxbyte[i]='\0';
		 for(j=0;j<token;j++)
		{
		    if(strcmp(rxbyte,p[j])==0)
		     {
			    count =1;
			    IOSET0=1<<LED2;
			    m1.databyteA=0x1;
	            m1.databyteB=0;
			    can2_tx(m1);
			    delay_ms(1000);
			    break;

			}
		}
		if((j==token)&&(count!=1))
		{
		            IOSET0=1<<LED1;
		            m1.databyteA=0x0;
	                m1.databyteB=0;
		            can2_tx(m1);
 		            delay_ms(1000);

		}
		 
	}
		   	
}

void can2_init(void)
{
  PINSEL1=0x14000;
  VPBDIV=1;//PCLK=60MHz
  C2MOD=0x01;
  AFMR=2;
  C2BTR=0x001C001D;
  C2MOD=0x00;
}
void can2_tx(CAN2_MSG m1)
{
  C2TID1=m1.id;
  C2TFI1=m1.dlc<<16;
  if(m1.rtr == 0)
  {
	C2TFI1 &= ~(1<<30);
	C2TDA1 = m1.databyteA;
	C2TDB1 = m1.databyteB;
  }
  else
  {
    C2TFI1 |= (1<<30);
  }
  C2CMR=(1<<0)|(1<<5);//Start Xmission & select Tx Buf1
	//C2CMR=(3<<0)|(1<<5);//Start Xmission & select Tx Buf1
	while((C2GSR&(1<<3))==0);//wait for data Xmission.


}
void can2_rx(CAN2_MSG *m1)
{
	   while((C2GSR&0x01) == 0);
	   m1->id=C2RID;
	   m1->dlc=(C2RFS>>16)&0xF;
	   m1->rtr=(C2RFS>>30)&0x1;
	   if(m1->rtr==0)
	   { //if data frame
		    m1->databyteA=C2RDA;
		    m1->databyteB=C2RDB;
	   }
	   C2CMR=(1<<2);//free receiver buffer(imp)
	   //while(C2GSR & 0x01); // Wait until released
     //delay_ms(10); 
}
void delay_ms(int ms)
{
  T0PR=60000-1;
  T0TCR=0x01;
  while(T0TC<ms);
  T0TCR=0x03;
  T0TCR=0x00;
}
void UART_init()
{
	  PINSEL0 |= 0x05;
		U0LCR=0x83;
		U0DLL=0x86;
		U0DLM=0x01;
	  U0LCR=0x03;
}
unsigned char UART_RX()
{
	while((U0LSR&1)==0);
	return U0RBR;
}
