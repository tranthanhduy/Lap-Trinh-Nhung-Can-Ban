//
// Smpl_PWM_Tone_Keypad 
//
// use Keypad to select a tone
// use PWM to output the tone
// Output : PWM1 (GPA13)

#include <stdio.h>																											 
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "Driver_PWM.h"
#include "DrvGPIO.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include "NUC1xx-LB_002\ScanKey.h"
#include "note_freq.h"
uint8_t number;
char keyinput[1];

void My_Scan_Key()
{
	if(DrvGPIO_GetBit(E_GPA,0) == 0)
		{
			DrvGPIO_SetBit(E_GPA, 3);
			if(DrvGPIO_GetBit(E_GPA,0) == 1)
			{
				number = 3;
				keyinput[0] = 'C';
			}
			else 
			{
				DrvGPIO_SetBit(E_GPA, 4);
				if(DrvGPIO_GetBit(E_GPA,0) == 1)
				{
					number = 6;
					keyinput[0] = 'F';
				}
				else 
				{
					number = 9;
					keyinput[0] = 'I';
				}
			}
			DrvGPIO_ClrBit(E_GPA, 3);
			DrvGPIO_ClrBit(E_GPA, 4);
		}
		
		
		else if(DrvGPIO_GetBit(E_GPA,1) == 0)
		{
			DrvGPIO_SetBit(E_GPA, 3);
			if(DrvGPIO_GetBit(E_GPA,1) == 1)
			{
				number = 2;
				keyinput[0] = 'B';
			}
			else 
			{
				DrvGPIO_SetBit(E_GPA, 4);
				if(DrvGPIO_GetBit(E_GPA,1) == 1)
				{
					number = 5;
					keyinput[0] = 'E';
				}
				else 
				{
					number = 8; 
					keyinput[0] = 'H';
				}
			}
			DrvGPIO_ClrBit(E_GPA, 3);
			DrvGPIO_ClrBit(E_GPA, 4);
		}
		
		
		
		else if(DrvGPIO_GetBit(E_GPA,2) == 0)
		{
			DrvGPIO_SetBit(E_GPA, 3);
			if(DrvGPIO_GetBit(E_GPA,2) == 1)
			{
				number = 1;
				keyinput[0] = 'A';
			}
			else 
			{
				DrvGPIO_SetBit(E_GPA, 4);
				if(DrvGPIO_GetBit(E_GPA,2) == 1)
				{
					number = 4;
					keyinput[0] = 'D';
				}
				else 
				{
					number = 7;
					keyinput[0] = 'G';
				}
			}
			DrvGPIO_ClrBit(E_GPA, 3);
			DrvGPIO_ClrBit(E_GPA, 4);
	}
}

int32_t main (void)
{

	uint16_t tone;
	uint8_t  duty_cycle;

	//Enable 12Mhz and set HCLK->12Mhz
	char TEXT0[16]="Final report !!!";
	char TEXT1[16]="Press Keypad ...";
	char TEXT2[16]="Keyin:        ";
	char TEXT3[16]="Duty:         ";
	duty_cycle = 100; 

	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1;
	SYSCLK->CLKSEL0.HCLK_S = 0;
	LOCKREG();

	
	Initial_panel();	
	clr_all_panel();
	
	InitPWM(1); // initialize PWM1, output pin = GPA13
    	
	DrvGPIO_Open(E_GPA,0,E_IO_QUASI);
	DrvGPIO_Open(E_GPA,1,E_IO_QUASI);
	DrvGPIO_Open(E_GPA,2,E_IO_QUASI);
	
	DrvGPIO_Open(E_GPA,3,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPA,4,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPA,5,E_IO_OUTPUT);
	     	
	DrvGPIO_ClrBit(E_GPA, 3);           
	DrvGPIO_ClrBit(E_GPA, 4);            
	DrvGPIO_ClrBit(E_GPA, 5);  		
	
 	print_lcd(0,TEXT0);
 	print_lcd(1,TEXT1); 
	while(1)
	{
		My_Scan_Key(); // Scan my keypad
		switch(number) {
			case 1 : tone=C5; duty_cycle=90; break;	// Led is converse -> HIGH state is OFF, LOW state is ON
			case 2 : tone=C5; duty_cycle=80; break;
			case 3 : tone=C5; duty_cycle=70; break;
			case 4 : tone=C5; duty_cycle=60; break;
			case 5 : tone=C5; duty_cycle=50; break;
			case 6 : tone=C5; duty_cycle=40; break;
			case 7 : tone=C5; duty_cycle=30; break;
			case 8 : tone=C5; duty_cycle=20; break;
			case 9 : tone=C5; duty_cycle=10; break;
			default: tone=0; break;
		}
		PWM_Freq(1,tone,duty_cycle); // set PWM output
	  sprintf(TEXT2+6,"%2c",keyinput[0]);
	  sprintf(TEXT3+6,"%4d",100-duty_cycle); 
 	  print_lcd(2, TEXT2);
 	  print_lcd(3, TEXT3);
	}
}
