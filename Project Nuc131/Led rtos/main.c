#include <stdio.h>
#include "NUC131.h"

#include "../FreeRTOSv10.2.1/FreeRTOS/Source/include/FreeRTOS.h"
#include "../FreeRTOSv10.2.1/FreeRTOS/Source/include/task.h"
#include "../FreeRTOSv10.2.1/FreeRTOS/Source/include/semphr.h"

#define PLL_CLOCK   50000000


//void vTask1( void *pvParameters );
//void vTask2( void *pvParameters );
//void vTask3( void *pvParameters );

//const char *pvTask1  = "Task1 is running.";
//const char *pvTask2  = "Task2 is running.";
//const char *pvTask3  = "Task3 is running.";


//SemaphoreHandle_t xSemaphore = NULL; // ##############Add this line if use semaphore


//void delay(int itime){
//         
//   int icompTime = 0;

//   for(icompTime=itime*1000;icompTime!=0;icompTime--);
//}

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

void vTask1( void *pvParameters )
{
		while(1){
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        GPIO_TOGGLE(PB1);
			//xSemaphoreGive(xSemaphore);
        vTaskDelay(1000/10/2/portTICK_RATE_MS);
    }
}
/*-----------------------------------------------------------*/
void vTask2( void *pvParameters )
{
		while(1){
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        GPIO_TOGGLE(PB11);
			//xSemaphoreGive(xSemaphore);
        vTaskDelay(1000/5/2/portTICK_RATE_MS);
    }
} 

void vTask3( void *pvParameters )
{

    while(1){
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        GPIO_TOGGLE(PB13);
			//xSemaphoreGive(xSemaphore);
        vTaskDelay(1000/5/2/portTICK_RATE_MS);
    }

}

void vTask4( void *pvParameters )
{

    while(1){
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        GPIO_TOGGLE(PB0);
			//xSemaphoreGive(xSemaphore);
        vTaskDelay(1000/5/2/portTICK_RATE_MS);
    }

}

int main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
		//config GPIO
		GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT1, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT0, GPIO_PMD_OUTPUT);
    /* Lock protected registers */
    SYS_LockReg();
		// Blinking slowly when don't press the button
	
//	xTaskCreate(vTask1, /* Pointer to the function that implements the task. */
//	"Task 1", /* Text name for the task.  This is to facilitate debugging only. */
//        configMINIMAL_STACK_SIZE, /* Stack depth in words. */
//	(void*)pvTask1,		/* We are not using the task parameter. */
//	1,			/* This task will run at priority 1. */
//	NULL );		/* We are not using the task handle. */
//configMINIMAL_STACK_SIZE
		xTaskCreate( vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
		xTaskCreate( vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
		xTaskCreate( vTask3, "Task 3", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
		xTaskCreate( vTask4, "Task 4", configMINIMAL_STACK_SIZE, NULL, 1, NULL );


	
//    // Start RTOS scheduler
    vTaskStartScheduler();
	
			return 0;
}





