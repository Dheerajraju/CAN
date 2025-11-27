#include<LPC21xx.h>
#include"LCD.h"
typedef unsigned int u32;
typedef unsigned char u8;

#define LED 17
#define USmsg 0x100
#define RFmsgid 0x101
#define LDRmsg 0x102
typedef struct 
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 databyteA;
	u32 databyteB;
}CAN2_MSG;
unsigned char rf1[2][20]={"ACCESS DENIED","ACCESS GRANTED"};
unsigned char rf2[2][20]={"TRY AGAIN","USER ID: ####"};

unsigned char ldr1[2][20]={"GOOD WEATHER","BAD WEATHER"};
unsigned char ldr2[2][20]={"LIGHT OFF","LIGHT ON"};
unsigned char od1[2][20]={"AREA CLEAR","ALERT! OBSTACLE"};
unsigned char od2[2][20]={"MOTOR RUN","MOTOR STOPPED"};
void can2_init(void);
void can2_tx(CAN2_MSG m1);
void can2_rx(CAN2_MSG *m1);

int main()
{
	CAN2_MSG m1;
    IODIR0 |=1<<LED;
	can2_init();
	LCD_init();
	LCD_command(0x80);
	LCD_STR("SYSTEM:");
	while(1)	  
	{
	      IOCLR0=1<<LED;
		  can2_rx(&m1);
			if((m1.id == RFmsgid))
			{
				IOSET0=1<<LED;	     
				LCD_command(0x01);
			    LCD_command(0x80);
				LCD_STR(rf1[m1.databyteA]);
				LCD_command(0xc0);
				LCD_STR(rf2[m1.databyteA]);
			}
			if((m1.id == LDRmsg))
			{
				IOSET0=1<<LED;	     
				LCD_command(0x01);
			              LCD_command(0x80);
				LCD_STR(ldr1[m1.databyteA]);
				LCD_command(0xc0);
				LCD_STR(ldr2[m1.databyteA]);
			}
			if((m1.id == USmsg))
			{
				IOSET0=1<<LED;	     
				LCD_command(0x01);
			        LCD_command(0x80);
				LCD_STR(od1[m1.databyteA]);
				LCD_command(0xc0);
				LCD_STR(od2[m1.databyteA]);
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
	   { 
	     //if data frame
	   	 m1->databyteA=C2RDA;
		   m1->databyteB=C2RDB;
	   }
	   C2CMR=(1<<2);//free receiver buffer(imp)
	   //while(C2GSR & 0x01); // Wait until released
                 delay_ms(10); 
}
