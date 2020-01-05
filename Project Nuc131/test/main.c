#include <stdint.h>
/* Kernel includes. */
#include "NUC131.h"                     // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

uint32_t SysCoreClock = 50000000;

void SysInit(void){
	
	UNLOCKREG();
	CLK->PWRCON = 
        CLK_PWRCON_IRC10K_EN_Msk |  /* LIRC */
        CLK_PWRCON_IRC22M_EN_Msk |  /* HIRC */
        CLK_PWRCON_XTL12M_EN_Msk;  /* 4-24 MHz Ext. Oscillator */
    CLK->PLLCON = CLK_PLLCON_50MHz_HXT;
    CLK->CLKSEL0 = 
        CLK_CLKSEL0_HCLK_S_PLL | /* HCLK from PLL */
        CLK_CLKSEL0_STCLK_S_HXT; /* SysTick from Ext. Osc...but test result from PLL */
    LOCKREG();

}

void vApplicationTickHook(void){
    /* This function will be called by each tick interrupt if
       configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
       added here, but the tick hook is called from an interrupt context, so
       code must not attempt to block, and only the interrupt safe FreeRTOS API
       functions can be used (those that end in FromISR()). */
}

                                              void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName){
    (void) pcTaskName;
    (void) pxTask;
    /* Run time stack overflow checking is performed if
       configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
       function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for(;;);
}
void vApplicationMallocFailedHook( void ){
    /* vApplicationMallocFailedHook() will only be called if
       configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
       function that will get called if a call to pvPortMalloc() fails.
       pvPortMalloc() is called internally by the kernel whenever a task, queue,
       timer or semaphore is created.  It is also called by various parts of the
       demo application.  If heap_1.c or heap_2.c are used, then the size of the
       heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
       FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
       to query the size of free heap space that remains (although it does not
       provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for(;;);
}
void vApplicationIdleHook( void ){
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
       to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
       task.  It is essential that code added to this hook function never attempts
       to block in any way (for example, call xQueueReceive() with a block time
       specified, or call vTaskDelay()).  If the application makes use of the
       vTaskDelete() API function (as this demo application does) then it is also
       important that vApplicationIdleHook() is permitted to return to its calling
       function, because it is the responsibility of the idle task to clean up
       memory allocated by the kernel to any task that has since been deleted. */
}

/* The period after which the check timer will expire provided no errors have
   been reported by any of the standard demo tasks.  ms are converted to the
   equivalent in ticks using the portTICK_PERIOD_MS constant. */
#define mainCHECK_TIMER_PERIOD_MS			( 500UL / portTICK_PERIOD_MS )

/* The period at which the check timer will expire if an error has been
   reported in one of the standard demo tasks.  ms are converted to the equivalent
   in ticks using the portTICK_PERIOD_MS constant. */
#define mainERROR_CHECK_TIMER_PERIOD_MS 	( 200UL / portTICK_PERIOD_MS )

/* The LED toggle by the check timer. */
#define mainCHECK_LED						( 3 )
/*-----------------------------------------------------------*/

void vParTestToggleLED(unsigned long ulLED){
    if(ulLED == 1){
        if((PB->DOUT & (1<<0)) == 0){
            PB->DOUT |= (1<<0);
        }else{
            PB->DOUT &= ~(1<<0);
        }
        
    }else if(ulLED == 2){
        if((PB->DOUT & (1<<1)) == 0){
            PB->DOUT |= (1<<1);
        }else{
            PB->DOUT &= ~(1<<1);
        }
        
    }else if(ulLED == 3){
        if((PB->DOUT & (1<<13)) == 0){
            PB->DOUT |= (1<<13);
        }else{
            PB->DOUT &= ~(1<<13);
        }
    }
}

/* See the description at the top of this file. */

void vTaskLED1(void *pvParameters){
	const TickType_t xDelay = (1000/10/2/portTICK_PERIOD_MS);
	while (1) {
		//vParTestToggleLED(1);
		GPIO_TOGGLE(PB1);
		vTaskDelay(xDelay);
	}
}

void vTaskLED2(void *pvParameters){
	const TickType_t xDelay = (1000/5/2/portTICK_PERIOD_MS);
	while (1) {
		//vParTestToggleLED(2);
		GPIO_TOGGLE(PB11);
		vTaskDelay(xDelay);
	}
}

void vTaskLED3(void *pvParameters){
	const TickType_t xDelay = (1000/5/2/portTICK_PERIOD_MS);
	while (1) {
		//vParTestToggleLED(2);
		GPIO_TOGGLE(PB13);
		vTaskDelay(xDelay);
	}
}

int main(void)
{
	//.............
	SYS_UnlockReg();
	SysInit();
		
		GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT1, GPIO_PMD_OUTPUT);

		SYS_LockReg();
			
		xTaskCreate(vTaskLED1,(const char*)"Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
		xTaskCreate(vTaskLED2,(const char*)"Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
		xTaskCreate(vTaskLED3,(const char*)"Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
       line will never be reached.  If the following line does execute, then there
       was	insufficient FreeRTOS heap memory available for the idle and/or timer
       tasks to be created.  See the memory management section on the FreeRTOS web
       site, or the FreeRTOS tutorial books for more details. */
}


