#include <libopencm3/stm32/rcc.h> 	// rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> 	// inputs outputs
#include <libopencm3/stm32/usart.h>

#include "uart_setup/uart_setup.hpp"						// конфигурация UART
#include "uart_data_processing/uart_data_processing.hpp"	// работа с данными из UART

#define SPEED 115200
#define WSIZE 8
#define MAX_STR_LEN 8

int main() {
	UART_Clock_Setup();
	UART_GPIO_Setup();
	USART2_Setup(SPEED, WSIZE);

	while (true) {
        UART_Read_String(MAX_STR_LEN);  // Вызываем в основном цикле
		
        if (Is_String_Ready()) {
            const char* str = Get_Received_String();	// получаем готовую строку
			
			UART_Send_String(USART2, "\r\n");			// переходим на новую строку
														// и возвращаем курсор в начало
			UART_Send_String(USART2, str);
			UART_Send_String(USART2, "\r\n");
        }
	}
}