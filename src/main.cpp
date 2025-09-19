#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>

#include "DATA_PROC/DATA_PROC.hpp"

// Глобальный объект для данных датчика
infrared_sensor_data sensor_data = {};
infrared_sensor_data* sensor = &sensor_data;

void usart2_isr(void) {
    UART_Processing(sensor);
}

// Обработчик прерывания DMA
// void dma1_channel7_isr() {
//     if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL7, DMA_TCIF)) {
//         dma_clear_interrupt_flags(DMA1, DMA_CHANNEL7, DMA_TCIF);
//         sensor->dma_transfer_complete = true;
//     }
// }

int main(void) {
    Clock_Setup();
    UART2_GYMCU90640_Setup();
    //UART2_Data_Setup();
    //DMA1_Setup(sensor);
    
    while (true) {
        // //Если передача завершена, готовим новые данные и запускаем снова
        // if (sensor->dma_transfer_complete) {
        //     sensor->dma_transfer_complete = false;
            
        //     // Запуск следующей передачи
        //     Start_DMA1_Transfer();
        // }
    }
}