/**
  ******************************************************************************
  * @file    Template/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "game.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"

#include <r3dfb.h>
#include <r3d.h>



#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/** @addtogroup Template
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t demoMode;

static void UARTEventTask1( void *pvParameters );
static void UARTEventTask1( void *pvParameters );
xTaskHandle xTest1Task;  //UART
xTaskHandle xTest2Task;		//game
xTaskHandle xTest3Task;  //always run
xTaskHandle xgame1Task;		//game
xTaskHandle xgame2Task;		//game
xTaskHandle xgame3Task;		//game

enum {
TASK_RUNNING,
TASK_STOP,
TASK_INTERRUPT
};
int last_command=1;
int SUART_Return=1;  //shell 
int GUART_Return=0;  //game

void
prvInit()
{
	//LCD init
	LCD_Init();
	IOE_Config();
	LTDC_Cmd( ENABLE );

	LCD_LayerInit();
	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	LCD_Clear( LCD_COLOR_BLACK );
	LCD_SetTextColor( LCD_COLOR_WHITE );

	//Button
	STM_EVAL_PBInit( BUTTON_USER, BUTTON_MODE_GPIO );

	//LED
	STM_EVAL_LEDInit( LED3 );
	
	//gryo
	gryo_init();


	//UART
	RCC_Configuration();
  GPIO_Configuration();
  USART1_Configuration();
 //USART1_puts("Game test\r\n");
}

static void GameEventTask1( void *pvParameters )
{
	while( 1 ){
		GAME_EventHandler1();
	}
}
/*
static void GameEventTask2( void *pvParameters )
{
	while( 1 ){
		GAME_EventHandler2();
	}
}
*/
static void GameEventTask3( void *pvParameters )
{
	while( 1 ){
		GAME_EventHandler3();
	}
}





static void GameTask( void *pvParameters )
{
	while( 1 ){
		GAME_Update();
		
		GAME_Render();
		vTaskDelay( 10 );
	}
}
static void ShellEventTask( void *pvParameters )  //for shell
{
		while (1)
		{	
			
			if(SUART_Return==0&&last_command!=0)  //shell get start command
			{		
		
		
				
				
				last_command=SUART_Return;
			}
						
		}
	
}
static void UARTEventTask( void *pvParameters )  //UART for shell
{
		while (1)
		{	
			SUART_Return=UART_EventHandler();
			if(SUART_Return==0)
			{
				xTaskCreate( GameTask, (signed char*) "GameEventTask1", 128, NULL, tskIDLE_PRIORITY + 1, &xgame1Task );
				xTaskCreate( GameEventTask1, (signed char*) "GameEventTask1", 128, NULL, tskIDLE_PRIORITY + 1, &xgame2Task );
				//xTaskCreate( GameEventTask2, (signed char*) "GameEventTask2", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
				xTaskCreate( GameEventTask3, (signed char*) "GameEventTask3", 128, NULL, tskIDLE_PRIORITY + 1, &xgame3Task );
				xTaskCreate( UARTEventTask1, (signed char*) "UARTEventTask1", 128, NULL, tskIDLE_PRIORITY + 1, &xTest2Task );  //for game
				vTaskSuspend(xTest1Task);		
				SUART_Return=1;
						
			}
		}
	
}


static void UARTEventTask1( void *pvParameters )  //for game
{
	while(1)
	{	
		GUART_Return=UART_EventHandler1();
		if(GUART_Return==1)
		{
				xTaskCreate( UARTEventTask, (signed char*) "UARTEventTask", 128, NULL, tskIDLE_PRIORITY + 1, &xTest1Task );	//for UART
				vTaskSuspend(xgame1Task);
				vTaskSuspend(xgame2Task);
				vTaskSuspend(xgame3Task);
				vTaskSuspend(xTest2Task);

GUART_Return=0;
				last_command=TASK_STOP;
		}
	}
}

//Main Function
int main(void)
{
	prvInit();

	if( STM_EVAL_PBGetState( BUTTON_USER ) )
		demoMode = 1;

		xTaskCreate( ShellEventTask, (signed char*) "ShellEventTask", 128, NULL, tskIDLE_PRIORITY + 1, &xTest3Task );  //for shell
		
		xTaskCreate( UARTEventTask, (signed char*) "UARTEventTask", 128, NULL, tskIDLE_PRIORITY + 1, &xTest1Task );	//for UART		

		
		vTaskStartScheduler();	
		
	//Call Scheduler
	//vTaskStartScheduler();
}
