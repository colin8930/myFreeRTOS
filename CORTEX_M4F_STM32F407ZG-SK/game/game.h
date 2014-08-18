#ifndef GAME_H
#define GAME_H

void
gryo_init();

void
gryo_update();

void
EventHandler1();

void
EventHandler2();

void
EventHandler3();

void
UART_EventHandler();

void
GAME_Update();

void
GAME_Render();

void 
RCC_Configuration();

void 
GPIO_Configuration();

void 
SART1_Configuration();

void USART1_puts(char*);

#endif
