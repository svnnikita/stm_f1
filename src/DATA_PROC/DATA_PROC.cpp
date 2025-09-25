#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>

#include "DATA_PROC/DATA_PROC.hpp"

#define BAUD_SPEED 115200
#define WORD_SIZE 8

// Определение функций для STM32F3
// тактирование
void Clock_Setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    //rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_USART2);
    //rcc_periph_clock_enable(RCC_DMA1);
}

/* Настройка UART2 для обмена данными с датчиком
 * По каналу GPIO_USART2_RX по прерываниям принимаем данные с датчика,
 * сохраняем их в буффер, и при помощи DMA отправляем их по каналу GPIO_USART2_TX в терминал */
void UART2_GYMCU90640_Setup(void) {

    // настраиваем выводы
    // LED для отображения принятых данных с датчика
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
                    GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);

    /* Setup GPIO pin GPIO_USART2_TX on GPIO port A for transmit. */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                    GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    /* Setup GPIO pin GPIO_USART2_RX on GPIO port A for receive. */
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                    GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);           

    // настраиваем UART2
    usart_set_baudrate(USART2, BAUD_SPEED);					// скорость передачи данных
    usart_set_databits(USART2, WORD_SIZE);					// размер слова
    usart_set_stopbits(USART2, USART_STOPBITS_1);			// количество стоповых битов
    usart_set_mode(USART2, USART_MODE_TX_RX);				// режим передачи данных
    usart_set_parity(USART2, USART_PARITY_NONE);			// без бита контроля
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);	// без управления потоком

    usart_enable_rx_interrupt(USART2);
    nvic_enable_irq(NVIC_USART2_IRQ);

    // Включение передачи по DMA для UART2
    // usart_enable_tx_dma(USART2);

    // включаем UART2
    usart_enable(USART2);
}

// // настраиваем UART2, по которому будем видеть обработанные данные с датчика
// void UART_Data_Setup(void)
// {
// 	// PA2 (TX) как выход с альтернативной функцией
//     gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
//                   GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

//     usart_set_baudrate(USART2, bod_speed);					// скорость передачи данных
// 	usart_set_databits(USART2, word_size);					// размер слова
// 	usart_set_stopbits(USART2, USART_STOPBITS_1);			// количество стоповых битов
// 	usart_set_mode(USART2, USART_MODE_TX);				// режим передачи данных
// 	usart_set_parity(USART2, USART_PARITY_NONE);			// без бита контроля
// 	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);	// без управления потоком

// 	usart_enable(USART2);
// }

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

// Настройки конфигурации DMA
// При помощи DMA отправляем полученные и записанные данные с датчика
// void DMA1_Setup(infrared_sensor_data* sensor) {
//     // Сброс и настройка DMA1 канала 4 для USART1_TX
//     dma_channel_reset(DMA1, DMA_CHANNEL7);
    
//     dma_set_peripheral_address(DMA1, DMA_CHANNEL7, (uint32_t)&USART2_DR);
//     dma_set_memory_address(DMA1, DMA_CHANNEL7, (uint32_t)sensor->frame_buffer);
//     dma_set_number_of_data(DMA1, DMA_CHANNEL7, DATA_SIZE);
    
//     dma_set_read_from_memory(DMA1, DMA_CHANNEL7);
//     dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL7);
//     dma_set_peripheral_size(DMA1, DMA_CHANNEL7, DMA_CCR_PSIZE_8BIT);
//     dma_set_memory_size(DMA1, DMA_CHANNEL7, DMA_CCR_MSIZE_8BIT);
//     dma_set_priority(DMA1, DMA_CHANNEL7, DMA_CCR_PL_LOW);
    
//     // Включение прерывания по завершению передачи
//     dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);
    
//     // Разрешение прерывания DMA
//     nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ);
// }

// Перезагрузка канала DMA
// void Start_DMA1_Transfer(void) {
//     dma_disable_channel(DMA1, DMA_CHANNEL7);
//     dma_set_number_of_data(DMA1, DMA_CHANNEL7, DATA_SIZE);
//     dma_clear_interrupt_flags(DMA1, DMA_CHANNEL7, DMA_TCIF);
//     dma_enable_channel(DMA1, DMA_CHANNEL7);
// }

// для приёма данных с датчика используем прерывания.
// отслеживаем новые данные в буфере UART, и если они есть,
// то поднимается флаг USART_FLAG_RXNE и срабатывает прерывание
void UART_Processing(infrared_sensor_data* sensor) {
    if (usart_get_flag(USART2, USART_FLAG_RXNE)) {

        sensor->frame_ready = false;        // кадр обновляется
        uint8_t data = usart_recv(USART2);
        //usart_send(USART2, data);
        
        // ПОИСК НАЧАЛА КАДРА (0x5A 0x5A)
        // если первый принятый байт равен 0x5A, то записываем этот байт и двигаемся
        // дальше по индексам буффера
        if (sensor->buffer_index == 0 && data == '0') {
            sensor->frame_buffer[sensor->buffer_index] = data;  // записываем 
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }
        // если второй байт равен 0x5A, то тоже его записываем
        else if (sensor->buffer_index == 1 && data == '1') {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        } 

        // ПОИСК БАЙТОВ КОЛИЧЕСТВА ПОЛЕЗНЫХ ДАННЫХ
        else if (sensor->buffer_index == 2 && data == '2') {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == 3 && data == '3') {
            sensor->frame_buffer[sensor->buffer_index] = data;
                
            sensor->buffer_index++;
        }

        // ПОИСК БАЙТОВ ТЕМПЕРАТУРЫ
        else if (sensor->buffer_index >= 4 && sensor->buffer_index < DATA_SIZE - 4) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }

        // ПОИСК БАЙТОВ СОБСТВЕННОЙ ТЕМПЕРАТУРЫ ДАТЧИКА
        else if (sensor->buffer_index == DATA_SIZE - 4) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == DATA_SIZE - 3) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }    

        // ПОИСК БАЙТОВ СОВОКУПНОЙ СУММЫ ПЕРВЫХ 771 СЛОВ (16 БИТ)
        else if (sensor->buffer_index == DATA_SIZE - 2) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            sensor->buffer_index++;
        }
        else if (sensor->buffer_index == DATA_SIZE - 1) {
            sensor->frame_buffer[sensor->buffer_index] = data;
            usart_send(USART2, sensor->frame_buffer[sensor->buffer_index]);
            
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

