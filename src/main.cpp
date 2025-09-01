#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include "uart_setup/uart_setup.hpp"						// конфигурация UART
#include "uart_data_processing/uart_data_processing.hpp"	// работа с данными из UART
#include "digitalFilter_operations/digitalFilter_operations.hpp"

#define SPEED 115200        // Константы ддя конфигурации UART
#define WSIZE 8

#define PRESC 8000          // Значения таймера
#define PERIOD 1000

void timer_setup(void) {
    rcc_periph_clock_enable(RCC_TIM2);
    timer_set_prescaler(TIM2, PRESC - 1);
    timer_set_period(TIM2, PERIOD - 1);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
    timer_enable_counter(TIM2);
    nvic_enable_irq(NVIC_TIM2_IRQ);
}


// Объявим буфферы для хранения задержек
struct FilterBuffer zbuff_a1b1 = { .buffer = {0}, 
                                   .in = zbuff_a1b1.buffer, 
                                   .out = zbuff_a1b1.buffer + 1 };

struct FilterBuffer zbuff_a2b2 = { .buffer = {0}, 
                                   .in = zbuff_a2b2.buffer, 
                                   .out = zbuff_a2b2.buffer + 1 };

struct FilterBuffer zbuff_a3b3 = { .buffer = {0}, 
                                   .in = zbuff_a3b3.buffer, 
                                   .out = zbuff_a3b3.buffer + 1 };

struct FilterBuffer zbuff_a4b4 = { .buffer = {0}, 
                                   .in = zbuff_a4b4.buffer, 
                                   .out = zbuff_a4b4.buffer + 1 };
  

struct out_values fout = {};       // промежуточные значения при умножении отсчета на коэффициент
struct out_values* fout_ptr = &fout;

struct coefs fcoefs = {233008, -321612, 203811, -50202, 512, 0, -1026, 0, 512};    // коэффициенты фильтра
struct coefs* fcoefs_ptr = &fcoefs; 

// Объявляем глобальные переменные для обработки в прерывании
int64_t X_in = 65536;
int64_t* X_in_ptr = &X_in;


// Обработчик прерывания таймера
void tim2_isr(void) {
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
    
        // Сбрасываем флаг прерывания
        timer_clear_flag(TIM2, TIM_SR_UIF);

        // Вычисляем выходной отсчет фильтра
        Calculating_Counts( fout_ptr, fcoefs_ptr, X_in_ptr, 
                            &zbuff_a1b1, &zbuff_a2b2, &zbuff_a3b3, &zbuff_a4b4);

        Moving_Pointers(&zbuff_a1b1);
        Moving_Pointers(&zbuff_a2b2);
        Moving_Pointers(&zbuff_a3b3);
        Moving_Pointers(&zbuff_a4b4);

        // Отправка результата через UART
        UART_Send_Decimal(USART2, fout_ptr->y_out);
        UART_Send_String(USART2, "\n\r");
    }
}

int main() {
	UART_Clock_Setup();
	UART_GPIO_Setup();
	USART2_Setup(SPEED, WSIZE);
    timer_setup();

    while (true) {
        __asm__("nop");
    }
}