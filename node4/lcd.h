//4bit LCD
#include<LPC21xx.H>
#define LCD_D 0xf0<<16
#define RS 1<<17
#define RW 1<<18
#define E 1<<19
void LCD_init(void);
void LCD_command(unsigned char);
void LCD_data(unsigned char);
void LCD_STR(unsigned char *p);
void LCD_integer(int);
void LCD_float(float);
void LCD_binary(unsigned int);
void delay_ms(unsigned int);
void LCD_scroll(unsigned char *p);
void LCD_rotate(unsigned char *p);
void CGRAM_write(unsigned char LUT[],int d);
void LCD_init(void)
{
  IODIR1|=LCD_D|RS|E|RW;
  LCD_command(0x01);
  LCD_command(0x02);
  LCD_command(0x0f);
  LCD_command(0x28);
}
void LCD_command(unsigned char cmd)
{
	IOCLR1=LCD_D;
	IOPIN1 =(cmd & 0xf0)<<16; 
  IOCLR1=RS;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
	
	IOCLR1=LCD_D;
	IOPIN1 =(cmd & 0x0f)<<20; 
  IOCLR1=RS;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
}
void LCD_data(unsigned char d)
{
  IOCLR1=LCD_D;
	IOPIN1 =(d & 0xf0)<<16; 
  IOSET1=RS;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
	
	IOCLR1=LCD_D;
	IOPIN1 =(d & 0x0f)<<20; 
  IOSET1=RS;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
}
void CGRAM_write(unsigned char LUT[],int d)
{
  unsigned char i;
  LCD_command(0x40);
  for(i=0;i<d;i++)
  LCD_data(LUT[i]);
}
void LCD_STR(unsigned char *p)
{
 unsigned char i;
 for(i=0;*p;i++)
 {
  LCD_data(*p++);
  if(i==16)
    LCD_command(0xc0);
 }
}
void LCD_integer(int num)
{
    unsigned char a[10];
	signed char i=0;
	while(num>0)
	{
	   a[i++]=num%10;
	   num=num/10;
	}
	for(--i;i>=0;i--)
		LCD_data(a[i]+48);
	                    
}
void LCD_float(float num)
{
   int n=num;
   LCD_integer(n);
   LCD_data('.');
   n=(num-n)*100;
   LCD_integer(n);
}
void delay_ms(unsigned int ms)
{
 T0PR=60000-1;
 T0TCR=0X01;
 while(T0TC<ms);
 T0TCR=0x03;
 T0TCR=0x00;
}
void LCD_binary(unsigned int num)
{
  int n,i;
  for (i=31;i>=0;i--)
  {
    if(i==15)
	  LCD_command(0xc0);
	n=((num>>i)&1);
	LCD_data(n+48);
  }
}
void LCD_scroll(unsigned char *p)
{
  while(1)
  {
    unsigned int i=0,j=0,len=0;
	while(p[i++])
	len++;
    for(i=0;i<16;i++,j=0)
	{
	   LCD_command(128+i);
	   while((j<len)&&(j<(16-i))){
	    LCD_data(p[j]);
		j++;
		}
	   delay_ms(200);
	   LCD_command(0x01);
	}
  }
}
void LCD_rotate(unsigned char *p)
{
 while(1)
  {
    unsigned int i=0,j=0,len=0;
	while(p[i++])
	len++;
    for(i=0;i<16;i++,j=0)
	{
	   LCD_command(128+i);
	   while((j<len)&&(j<(16-i)))
	   {
	    LCD_data(p[j]);
		j++;
		}
	   if(j<len)
	   {
	     LCD_command(0x80);
		   while(j<len)
		   {
		    LCD_data(p[j]);
			j++;
			}
		}
	   delay_ms(200);
	   LCD_command(0x01);
	}
  }
}
