#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "module_processing/module_processing.hpp"

#define BAUD_SPEED 115200
#define WORD_SIZE 8

// тактирование
void Clock_Setup(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_USART2);
}

// настройка UART для обмена данными с датчиком
void UART_GYMCU90640_Setup(void) {
	// настраиваем выводы
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9|GPIO10);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9|GPIO10);

    // настраиваем UART
	usart_set_baudrate(USART1, BAUD_SPEED);					// скорость передачи данных
	usart_set_databits(USART1, WORD_SIZE);					// размер слова
	usart_set_stopbits(USART1, USART_STOPBITS_1);			// количество стоповых битов
	usart_set_mode(USART1, USART_MODE_TX_RX);				// режим передачи данных
	usart_set_parity(USART1, USART_PARITY_NONE);			    // без бита контроля
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);	// без управления потоком

    // включаем UART
	usart_enable(USART1);
}

// настраиваем UART, по которому будем видеть обработанные данные с датчика
void UART_Data_Setup(void)
{
	// настраиваем выводы
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO14);

    // настраиваем UART
	usart_set_baudrate(USART2, BAUD_SPEED);					// скорость передачи данных
	usart_set_databits(USART2, WORD_SIZE);					// размер слова
	usart_set_stopbits(USART2, USART_STOPBITS_1);			// количество стоповых битов
	usart_set_mode(USART2, USART_MODE_TX);				// режим передачи данных
	usart_set_parity(USART2, USART_PARITY_NONE);			    // без бита контроля
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);	// без управления потоком

    // включаем UART
	usart_enable(USART2);
}

// // Отправляем строку символов через интерфейс USART в блокирующем режиме
// void UART_Send_String(uint32_t usart, char* str) {
//     while (*str != '\0') {  						    // пока не достигнут конец строки
//         usart_send_blocking(usart, (uint8_t)(*str)); 	// передаем текущий символ
//         str++; 											// переходим к следующему символу
//     }
// }

// // функция для отправки команд
// void USART_Send_Command(uint8_t *data) {
//     while (*data != '\0') {  			    // пока не достигнут конец строки
//         usart_send_blocking(USART1, *data); // передаем текущий символ
//         data++; 							// переходим к следующему символу
//     }
// }

// для приёма данных с датчика используем прерывания.
// отслеживаем новые данные в буфере UART, и если они есть,
// то поднимается флаг USART_FLAG_RXNE и срабатывает прерывание
void UART_Processing(infrared_sensor_data* sensor) {
    if (usart_get_flag(USART1, USART_FLAG_RXNE)) {

        sensor->frame_ready = false;        // кадр обновляется
        uint8_t data = usart_recv(USART1);
        
        // ПОИСК НАЧАЛА КАДРА (0x5A 0x5A)
        // если первый принятый байт равен 0x5A, то записываем этот байт и двигаемся
        // дальше по индексам буффера
        if (sensor->buffer_index == 0 && data == 0x5A) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }
        // если второй байт равен 0x5A, то тоже его записываем
        else if (sensor->buffer_index == 1 && data == 0x5A) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        } 

        // ПОИСК БАЙТОВ КОЛИЧЕСТВА ПОЛЕЗНЫХ ДАННЫХ
        else if (sensor->buffer_index == 2 && data == 0x02) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == 3 && data == 0x06) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }

        // ПОИСК БАЙТОВ ТЕМПЕРАТУРЫ
        else if (sensor->buffer_index >= 4 && sensor->buffer_index < DATA_SIZE - 4) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }

        // ПОИСК БАЙТОВ СОБСТВЕННОЙ ТЕМПЕРАТУРЫ ДАТЧИКА
        else if (sensor->buffer_index == DATA_SIZE - 4) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == DATA_SIZE - 3) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }    

        // ПОИСК БАЙТОВ СОВОКУПНОЙ СУММЫ ПЕРВЫХ 771 СЛОВ (16 БИТ)
        else if (sensor->buffer_index == DATA_SIZE - 2) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == DATA_SIZE - 1) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            
            sensor->frame_ready = true; // кадр готов
            sensor->buffer_index = 0;   // обнуляем индекс буффера
        }
        else {
            // Сброс при ошибке синхронизации
            sensor->buffer_index = 0;
        }
    }
}

// обработка  кадра
void Frame_Processing(infrared_sensor_data* sensor, ready_made_data* ready_data) {

    if (sensor->frame_ready == true) {   // если кадр готов

        // обрабатываем начало кадра
        if (sensor->frame_buffer[0] == 0x5A && sensor->frame_buffer[1] == 0x5A) {
            // ДЛЯ ОТПРАВКИ СООБЩЕНИЙ ПО USART2 МОЖНО ИСПОЛЬЗОВАТЬ DMA
            // здесь должна быть функция вывода сообщения в терминал
        }

        // обрабатываем и вычисляем количество полезной информации
        else if (sensor->frame_buffer[2] == 0x02 && sensor->frame_buffer[3] == 0x06) {
            ready_data->data_volume = sensor->frame_buffer[3]*256 + sensor->frame_buffer[2];
            // вывод значения в терминал
        }

        // обрабатываем и вычисляем температуру
        else if (sensor->frame_buffer[4] != 0) {
            for (uint8_t row = 0; row < ROWS; row++) {                         
                for (uint8_t col = 0; col < COLS; col++) {
                    uint16_t i = 4;
                    // идем по столбцам справа налево,
                    // а по строкам сверху вниз
                    ready_data->temperature[ROWS - row - 1][col] =              
                                        ((sensor->frame_buffer[i + 1] * 256) + 
                                          sensor->frame_buffer[i]) * 0.01;
                    i +=2;
                }
            }          
            // вывод значения в терминал
        }

        // обрабатываем собственную температуру датчика
        else if (sensor->frame_buffer[DATA_SIZE - 4] != 0 && sensor->frame_buffer[DATA_SIZE - 3] != 0) {
            ready_data->ambient_temperature = ((sensor->frame_buffer[DATA_SIZE - 3] * 256) + 
                                          sensor->frame_buffer[DATA_SIZE - 4]) * 0.01;
        }

        // вычисляем сумму всего кадра
        else if (sensor->frame_buffer[DATA_SIZE - 2] != 0 && sensor->frame_buffer[DATA_SIZE - 1] != 0) {
            ready_data->cumulative_sum = (sensor->frame_buffer[DATA_SIZE - 1] * 256) + 
                                          sensor->frame_buffer[DATA_SIZE - 2];
            // вывод значения в терминал
            sensor->frame_ready == false;
        }
    }
}
