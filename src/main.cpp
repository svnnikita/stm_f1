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

struct FilterBuffer zbuff = {
    .buffer = {0}, 
    .in = zbuff.buffer, 
    .out = zbuff.buffer + 1  // Или другую начальную позицию
};

struct out_values fout = {};       // промежуточне значений
struct out_values* fout_ptr = &fout; 

struct coefs fcoefs = {3276, 24576, -11469};    // коэффициенты фильтра
struct coefs* fcoefs_ptr = &fcoefs; 

// Объявляем глобальные переменные для обработки в прерывании
int32_t X_in = 16384;
int32_t* X_in_ptr = &X_in;

// Обработчик прерывания таймера
void tim2_isr(void) {
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        // Сбрасываем флаг прерывания
        timer_clear_flag(TIM2, TIM_SR_UIF);

        // Вычисляем один отсчет фильтра
        fout_ptr->b0_out = mult_p(&fcoefs_ptr->b0, X_in_ptr);   // Умножаем вход на B0
        fout_ptr->b0_out /= *X_in_ptr;
       
        fout_ptr->y_out = sum_p(&fout_ptr->b0_out, zbuff.out);    // Результат проходит на выход
        
        fout_ptr->a1_out = mult_p(&fout_ptr->y_out, &fcoefs_ptr->a1);   // Y умножается на A1
        fout_ptr->a1_out /= *X_in_ptr;
        
        fout_ptr->b1_out = mult_p(&fcoefs_ptr->b1, X_in_ptr);   // X умножается на B1
        fout_ptr->b1_out /= *X_in_ptr;

        *(zbuff.in) = sum_p(&fout_ptr->a1_out, &fout_ptr->b1_out);            // Сумма произведений записывается в Z.in
        
        // Циклическое перемещение указателей буфера
        zbuff.in = (zbuff.in == &zbuff.buffer[BUFF_SIZE - 1]) ? 
                        zbuff.buffer : 
                        zbuff.in + 1;

        zbuff.out = (zbuff.out == &zbuff.buffer[BUFF_SIZE - 1]) ? 
                         zbuff.buffer : 
                         zbuff.out + 1;
        
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