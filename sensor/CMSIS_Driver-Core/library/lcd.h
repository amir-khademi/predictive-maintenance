#ifndef lcd_INCLUDED_
#define lcd_INCLUDED_


#ifdef    LCD_LPC2368
#ifdef    LCD_PORT_0
#define   PIO_enable       PINSEL0
#define   PIO_SetOutput    FIO0SET
#define   PIO_ClearOutput  FIO0CLR
#define   PIO_CfgOutput    FIO0DIR
#endif

#ifdef    LCD_PORT_1
#define   PIO_enable       PINSEL2
#define   PIO_SetOutput    FIO1SET
#define   PIO_ClearOutput  FIO1CLR  
#define   PIO_CfgOutput    FIO1DIR 
#endif

#ifdef    LCD_PORT_2
#define   PIO_enable       PINSEL4
#define   PIO_SetOutput    FIO2SET
#define   PIO_ClearOutput  FIO2CLR
#define   PIO_CfgOutput    FIO2DIR
#endif

#ifdef    LCD_PORT_3
#define   PIO_enable        PINSEL6
#define   PIO_SetOutput     FIO3SET 
#define   PIO_ClearOutput   FIO3CLR
#define   PIO_CfgOutput     FIO3DIR
#endif

#ifdef    LCD_PORT_4
#define   PIO_enable        PINSEL8
#define   PIO_SetOutput     FIO4SET
#define   PIO_ClearOutput   FIO4CLR
#define   PIO_CfgOutput     FIO4DIR
#endif
#endif

#ifdef    LCD_LPC1768
#ifdef    LCD_PORT_0
#define   PIO_enable        PINSEL0
#define   PIO_SetOutput     LPC_GPIO0->FIOSET
#define   PIO_ClearOutput   LPC_GPIO0->FIOCLR
#define   PIO_CfgOutput     LPC_GPIO0->FIODIR
#endif

#ifdef    LCD_PORT_1
#define   PIO_enable        PINSEL2
#define   PIO_SetOutput     LPC_GPIO1->FIOSET   
#define   PIO_ClearOutput   LPC_GPIO1->FIOCLR  
#define   PIO_CfgOutput     LPC_GPIO1->FIODIR
#endif

#ifdef    LCD_PORT_2
#define   PIO_enable        PINSEL4
#define   PIO_SetOutput     LPC_GPIO2->FIOSET   
#define   PIO_ClearOutput   LPC_GPIO2->FIOCLR  
#define   PIO_CfgOutput     LPC_GPIO2->FIODIR
#endif


#ifdef    LCD_PORT_3
#define   PIO_enable       PINSEL6
#define   PIO_SetOutput    LPC_GPIO3->FIOSET   
#define   PIO_ClearOutput  LPC_GPIO3->FIOCLR  
#define   PIO_CfgOutput    LPC_GPIO3->FIODIR
#endif

#ifdef    LCD_PORT_4
#define   PIO_enable       PINSEL8
#define   PIO_SetOutput    LPC_GPIO4->FIOSET   
#define   PIO_ClearOutput  LPC_GPIO4->FIOCLR  
#define   PIO_CfgOutput    LPC_GPIO4->FIODIR
#endif
#endif



#define F_CPU   12000000

#define clcd_minDelay()   cwait(F_CPU/80000)
#define clcd_Delay()   cwait(F_CPU/8000)
#define clcd_maxDelay()   cwait(F_CPU/12)

void lcd_init(void);
void cwait (volatile int t);
void lcd_putchar(char ch);
void lcd_command(char cmd);
void lcd_gotoxy(char x, char y);
void lcd_clear(void);
void lcd_clear_line(char y);
void lcd_shift_left(char n);
void lcd_shift_right(char n);
void lcd_puts(int num);
void lcd_putsf(char* data);
void lcd_define_char(const char *pc,char char_code);
//============================================================================
void cwait (volatile int t) {
  for (;t; t--);
}
// ============================================================================
void setD4567(char val)
{
   PIO_ClearOutput =((1<<LCD_DB4)|(1<<LCD_DB5)|(1<<LCD_DB6)|(1<<LCD_DB7));
   if (val & 0x80)
      PIO_SetOutput =(1<<LCD_DB7)   ;
   if (val & 0x40)
      PIO_SetOutput =(1<<LCD_DB6);
   if (val & 0x20)
   PIO_SetOutput =(1<<LCD_DB5);
   if (val & 0x10)
   PIO_SetOutput =(1<<LCD_DB4);
}
// ============================================================================
void lcd_putchar(char ch)
{
   PIO_SetOutput =(1<<LCD_RS);
   setD4567(ch);
   PIO_SetOutput=(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput=(1<<LCD_E);
   clcd_minDelay();
   setD4567(ch<<4);
   PIO_SetOutput=(1<<LCD_E);
   clcd_minDelay();
PIO_ClearOutput=(1<<LCD_E);
   clcd_Delay();
}
// ============================================================================
void lcd_command(char cmd)   //Sends Command to LCD
{
   PIO_ClearOutput=(1<<LCD_RS);
   setD4567(cmd);
   PIO_SetOutput =(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput=(1<<LCD_E);
   clcd_minDelay();
   setD4567(cmd<<4);
   PIO_SetOutput =(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput =(1<<LCD_E);
   clcd_Delay();
}
// ============================================================================
void lcd_init()
{
   PIO_CfgOutput = ((1<<LCD_RS)|(1<<LCD_RW)|(1<<LCD_E)|(1<<LCD_DB4)|(1<<LCD_DB5)|(1<<LCD_DB6)|(1<<LCD_DB7));
	 PIO_ClearOutput = (1<<LCD_RW);
   clcd_Delay();
   setD4567(0);
   setD4567((1<<5)|(1<<4));
   PIO_SetOutput=(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput =(1<<LCD_E);
   clcd_Delay();
   setD4567((1<<5)|(1<<4));
   PIO_SetOutput =(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput =(1<<LCD_E);
   clcd_Delay();
   setD4567(1<<5);
   PIO_SetOutput =(1<<LCD_E);
   clcd_minDelay();
   PIO_ClearOutput =(1<<LCD_E);
   clcd_Delay();
   lcd_command(0x28);
   lcd_command(0x0c);
   clcd_Delay();
}
//============================================================================
void lcd_gotoxy(char y, char x)   //Cursor to X Y position
{
   char DDRAMAddr;
x=x-1;
   switch(y)
   {
      case 1: DDRAMAddr = 0x00+x; break;
      case 2: DDRAMAddr = 0x40+x; break;
      case 3: DDRAMAddr = 0x14+x; break;
      case 4: DDRAMAddr = 0x54+x; break;
      default: DDRAMAddr = 0x00+x;
   }
   lcd_command(1<<7 | DDRAMAddr);
   clcd_Delay();
}
// ============================================================================
void lcd_define_char(const char *pc,char char_code)
{
   char a , i;
   a = ((char_code<<3)|0x40) & 0xff;
   for (i = 0; i < 8 ;i++)
   {
      lcd_command(a++);
      clcd_Delay();
      lcd_putchar(pc[i]);
      clcd_Delay();
   }
}
//=============================================================================
void lcd_clear(void)            //Clears LCD
{
   lcd_command(0x01);
   clcd_Delay();
   lcd_command(0x02);
   clcd_maxDelay();
}
//=============================================================================
void lcd_shift_left(char n)   //Scrol n of characters Right
{
   char i;
   for (i = 0 ; i < n ; i++)
   {
      lcd_command(0x1E);
      clcd_Delay();
   }
}
//=============================================================================
void lcd_shift_right(char n)   //Scrol n of characters Left
{
   char q;
   for (q = 0 ;q < n ; q++)
   {
      lcd_command(0x18);
      clcd_Delay();
   }
}
//============================================================================
 void lcd_puts(int num)
{
   int i1,j1;
   int p,f=0;
   char ch[5];
 for(i1=0;i1<5;i1++)
   {
     p=1;
     for(j1=4-i1;j1>0;j1--)
       p = p*10;  
     ch[i1] = (num/p);
    if (num>=p && !f){
     f=1;
    }
     num =num - ch[i1] *p ;
    ch[i1] = ch[i1] +48;
     if(f) lcd_putchar(ch[i1]);
   }
}
// ============================================================================
void lcd_putsf(char* data)   //Outputs string to LCD
{
   char *p;
   p = data;
   if (!p)
      return;
   for(; *p ; )
   {
      lcd_putchar(*p);
      p++;
   }
}
//=============================================================================
void Display_off(void)
{
   lcd_command(0x08);
   clcd_Delay();
}
//=============================================================================
void Display_on(void)
{
   lcd_command(0x0C);
   clcd_Delay();
}
//=============================================================================
void cursor_off(void)
{
   lcd_command(0x0C);
   clcd_Delay();
}
//=============================================================================
void cursor_on(void)
{
   lcd_command(0x0E);
   clcd_Delay();
}
//=============================================================================
void cursor_blink(void)
{
   lcd_command(0x0F);
   clcd_Delay();
}
#endif
