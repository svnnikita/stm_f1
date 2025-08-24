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

// Функция для отправки числа через USART
void UART_Send_Decimal(uint32_t usart, int32_t num) {
    char buffer[12];  // Достаточно для int32_t (-2147483648)
    char* ptr = buffer + sizeof(buffer) - 1;
    uint8_t is_negative = 0;
    uint32_t abs_num;

    *ptr = '\0';  // Завершающий нулевой байт
    ptr--;

    // Обработка отрицательных чисел
    if (num < 0) {
        is_negative = 1;
        abs_num = (num == INT32_MIN) ? (uint32_t)INT32_MAX + 1 : (uint32_t)(-num);
    } else {
        abs_num = (uint32_t)num;
    }

    // Преобразование абсолютного значения в строку
    if (abs_num == 0) {
        *ptr = '0';
    } else {
        while (abs_num > 0) {
            *ptr = (abs_num % 10) + '0';
            abs_num /= 10;
            ptr--;
        }
        ptr++;  // Коррекция указателя на первую цифру
    }

    // Добавление знака минус для отрицательных чисел
    if (is_negative) {
        ptr--;
        *ptr = '-';
    }

    // Отправка строки
    UART_Send_String(usart, ptr);
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

