#include <libopencm3/stm32/rcc.h> 	// rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> 	// inputs outputs
#include <libopencm3/stm32/usart.h>

#include "uart_setup/uart_setup.hpp"

#define SPEED 115200
#define WSIZE 8

// Отправляем строку символов через интерфейс USART в блокирующем режиме
void UART_Send_String(uint32_t usart, const char *str) {
    while (*str != '\0') {  									// пока не достигнут конец строки
        usart_send_blocking(usart, (uint8_t)(*str)); 			// передаем текущий символ
        str++; 													// переходим к следующему символу
    }
}

int main() {
	UART_Clock_Setup();
	UART_GPIO_Setup();
	USART2_Setup(SPEED, WSIZE);

	while (true) {
		if (usart_get_flag(USART2, USART_SR_RXNE)) {
			uint16_t data = usart_recv(USART2);	// записываем полученное значение

			usart_send_blocking(USART2, data);

			switch (data)
			{
			case 'A':
				UART_Send_String(USART2, "\r\nПс, парень, хочешь немного денег?\r\n");		
				break;
			case 'B':
				UART_Send_String(USART2, "\r\nПривет, как дела?\r\n");	
				break;
			case 'C':
				UART_Send_String(USART2, "\r\nАХААХА\r\n");	
				break;
			default:
				break;
			}
		}
	}
}