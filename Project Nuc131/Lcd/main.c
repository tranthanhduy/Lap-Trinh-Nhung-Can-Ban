//
// Smpl_GPIO_QC12864B	: serial interfacing with LCD 128x64 module, GPIO emulate SPI
//	
// LCD QC12864B module
// pin 1 : Vss   to Gnd
// pin 2 : Vcc   to Vcc+5V
// pin 3 : 0V    to N.C.
// pin 4 : CS-RS-DI    to NUC140 GPC8 /pin61 emulate SPI_CS -> PA12
// pin 5 : SID->MOSI->r/w   to NUC140 GPC11/pin58 emulate SPI_DO -> PA14
// pin 6 : SCLK->E  to NUC140 GPC9 /pin60 emulate SPI_CLK -> PA13
// pin 7~14: N.C.
// pin 15: PSB   to Gnd (0=Serial-port mode, 1=Parallel-port mode)
// pin 16: N.C.
// pin 17: RST   to Vcc
// pin 18: N.C.
// pin 19: LED_A to +5V (Backlight Anode)
// pin 20: LED_K to Gnd (Backlight Cathode)

#include <stdio.h>
#include "NUC131.h"

#define PLL_CLOCK   50000000

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
UNLOCKREG();
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    //CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    //CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
LOCKREG();
}
void Init_SPI(void)
{
	GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT); // SPI_CS
	GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT); // SPI_CLK
	GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT); // SPI_DO
	PA12 = 0;
	PA13 = 0;
	PA14 = 0;
}

void SPI_Write(unsigned char zdata)
{
	unsigned int i;
	for(i=0; i<8; i++)
	{
		if((zdata << i) & 0x80)
		{
			PA14 = 1; // SID = 1;
		}
		else 
		{
			PA14 = 0; // SID = 0;
		}
		PA13 = 0; //SCLK = 0;
		CLK_SysTickDelay(10);
		PA13 = 1; //SCLK = 1;
	}
}

void SPI_CS(uint8_t pol)
{
	if (pol!=0) PA12 = 1;
	else        PA12 = 0;
}

void LCD_WriteCommand(uint8_t cmd)
{
	SPI_CS(1);
	SPI_Write(0xF8);
	SPI_Write(0xF0 & cmd);
	SPI_Write(0xF0 & (cmd<<4));
	SPI_CS(0);
}

void LCD_WriteData(unsigned char data)
{
	SPI_CS(1);
	SPI_Write(0xFA);
	SPI_Write(0xF0 & data);
	SPI_Write(0xF0 & (data<<4));
	SPI_CS(0);
}

void Init_QC12864B(void)
{
	LCD_WriteCommand(0x30);
	CLK_SysTickDelay(50);
	LCD_WriteCommand(0x0c);
	CLK_SysTickDelay(50);
}

void Clear_QC12864B(void)
{
	LCD_WriteCommand(0x01);
}

//void Print_QC12864B(uint8_t line, unsigned char *string)
void Print_QC12864B(uint8_t line, unsigned char *string)
{
	uint8_t i, addr;
	if      (line==0) addr = 0x80;
	else if (line==1) addr = 0x90;
	else if (line==2) addr = 0x88;
	else if (line==3) addr = 0x98;
	else              addr = 0x80;
	LCD_WriteCommand(addr);
	for (i=0; i<16; i++) LCD_WriteData(*string++);
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
   /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

    Init_SPI();
	Init_QC12864B();
	Clear_QC12864B();
	CLK_SysTickDelay(500);
								  	
  Print_QC12864B(0,"Lang Tu        ");
	Print_QC12864B(1,"Da Tinh              ");
	Print_QC12864B(2,"Yeu Em               ");
	Print_QC12864B(3,"Lan Cuoi             ");
}


