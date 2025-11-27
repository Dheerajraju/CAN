#include<LPC21xx.h>

typedef unsigned int u32;


#define light 19
#define USmsg 0x100
#define trig (1<<8)

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
unsigned int echo(void);
void delay_us(int ms);
int main()
{
    CAN2_MSG m1;
	unsigned int distance;
	float distance_cm;
	can2_init();
	m1.rtr=0;//data frame
	m1.dlc=4;
	m1.id=USmsg;
	IODIR0=1<<light|trig;
	while(1)
	{
		IOSET0=trig;
	    delay_ms(10);
	   IOCLR0=trig;
	   distance=echo();
       distance_cm=distance/58;
	
		if(distance_cm<=20)
		{
		               	
			 m1.databyteA=0x1;
             m1.databyteB=0;
			 can2_tx(m1);
			 delay_ms(200);
			 IOSET0=1<<light;
			  
		 }
		 else
		   	  IOCLR0=1<<light;
	}
}
 unsigned int echo()
{ 
	unsigned int time;
	while(((IOPIN0>>9)&1)==0);
	T0PR=15-1;
	T0TCR=0X02;
	T0TCR=0X01;
	while(((IOPIN0>>9)&1)==1);
	T0TCR=0X00;
	time=T0TC;
	T0TCR=0X02;
	return time;	
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
   while(!(C2SR & (1<<2)));
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
}
void delay_ms(int ms)
{
  T0PR=60000-1;
  T0TCR=0x01;
  while(T0TC<ms);
  T0TCR=0x03;
  T0TCR=0x00;
}
void delay_us(int ms)
{
  T0PR=60-1;
  T0TCR=0x01;
  while(T0TC<ms);
  T0TCR=0x03;
  T0TCR=0x00;
}
