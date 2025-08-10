#include <libopencm3/stm32/rcc.h>
//#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "uart_data_processing/uart_data_processing.hpp"

// Максимальная длина строки
#define MAX_STR_LEN_CONST 256

// Отправляем строку символов через интерфейс USART в блокирующем режиме
void UART_Send_String(uint32_t usart, const char *str) {
    while (*str != '\0') {  						    // пока не достигнут конец строки
        usart_send_blocking(usart, (uint8_t)(*str)); 	// передаем текущий символ
        str++; 											// переходим к следующему символу
    }
}

// Глобальные переменные для сохранения состояния
static char buffer[MAX_STR_LEN_CONST];
static uint8_t idx = 0;
static bool string_complete = false;

// Функция для чтения строки из 8+1 символов (8 символов + нуль-терминатор)
void UART_Read_String(uint16_t MAX_STR_LEN) {
    if (usart_get_flag(USART2, USART_SR_RXNE)) {
        uint8_t data = usart_recv(USART2);
        
        // Эхо-отправка символа
        usart_send_blocking(USART2, data);

        // Обработка конца строки
        if (data == '\r' || data == '\n') {
            buffer[idx] = '\0';  				// Добавляем нуль-терминатор
            string_complete = true;				// состояние строки -- готова
            UART_Send_String(USART2, buffer);  	// Отправка полной строки
            idx = 0;  							// Сброс индекса для новой строки
        }
		
        // Обработка обычных символов
        else if (idx < MAX_STR_LEN - 1) {
            buffer[idx] = data;
			idx++;
        }

        // Обработка backspace
        else if (data == '\b') {
            if (idx > 0) {
                idx--;
                usart_send_blocking(USART1, '\b');
            }
        }
    }
}

// Функция для проверки готовности строки
bool Is_String_Ready(void) {
    return string_complete;
}

// Функция для получения строки
const char* Get_Received_String(void) {
    string_complete = false;
    return buffer;
}

