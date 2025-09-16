#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
//#include <libopencm3/stm32/usart.h>
//#include <libopencm3/stm32/timer.h>
//#include <libopencm3/cm3/nvic.h>

//#include "module_processing/module_processing.hpp"

// // для приёма данных с датчика используем прерывания
// void usart1_isr(void) {
    
// }



int main(void) {

    rcc_periph_clock_enable(RCC_GPIOE);

    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);

    // GPIO_USARTx_RCC_Setup();
    // UART_GYMCU90640_Setup();
    // UART_Data_Setup();
    
    // отправляем команду установки скорости передачи
    // uint8_t command[] = {0xA5, 0x15, 0x02, 0xBC};
    // USART_Send_Command(command);

    while (true) {
        
        gpio_toggle(GPIOE, GPIO15);
        for(volatile uint32_t i =0; i < 2'000'000; i +=2) {
            __asm__("nop");
        }
    }
}