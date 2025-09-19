#include <libopencm3/stm32/rcc.h> 
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>

#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include "test/setup.hpp"

// настраиваем тактирование
void clock_setup(void)
{
    //rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE12_72MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA); // ОБЯЗАТЕЛЬНО ВКЛЮЧАЕМ ТАКТИРОВАНИЕ

	rcc_periph_clock_enable(RCC_USART2);

    rcc_periph_clock_enable(RCC_TIM2);

	// включаем тактирование DMA1
	rcc_periph_clock_enable(RCC_DMA1);
}

void timer_setup(void)
{
    // Настраиваем таймер TIM2 на частоту 1 Гц (1 раз в секунду)
    timer_set_prescaler(TIM2, 8000 - 1); // 72 МГц / 7200 = 10 КГц
    timer_set_period(TIM2, 175 - 1);   // 10 КГц / 10000 = 1 Гц
    
    // Включаем прерывание по обновлению таймера
    timer_enable_irq(TIM2, TIM_DIER_UIE);
    
    // Включаем таймер
    timer_enable_counter(TIM2);
    
    // Включаем прерывание таймера в NVIC
    nvic_enable_irq(NVIC_TIM2_IRQ);
}

// настройки UART.
void uart_setup(void)
{
	// PA2 (TX) как выход с альтернативной функцией
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

	// конфигурация UART
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	usart_enable(USART2);

    nvic_set_priority(NVIC_DMA1_CHANNEL7_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ);
}

// используем канал 7 USART2_TX для отправки обработанных чисел и отладочной информации
void DMA_CH7_USART2_TX_Setup(char * tx_buffer, uint8_t BUFFER_SIZE) {
	/* Reset DMA channel*/
	dma_channel_reset(DMA1, DMA_CHANNEL7);

	// указываем адрес регистра данных USART2, куда DMA будет пересылать данные
	dma_set_peripheral_address(DMA1, DMA_CHANNEL7, (uint32_t)&USART2_DR);

	// задаем начальный адрес массива в памяти, откуда DMA будет брать данные
	dma_set_memory_address(DMA1, DMA_CHANNEL7, (uint32_t)tx_buffer);

	// устанавливаем количество байт для передачи
	dma_set_number_of_data(DMA1, DMA_CHANNEL7, BUFFER_SIZE);

	// DMA будет читать данные из памяти и записывать в периферию
	dma_set_read_from_memory(DMA1, DMA_CHANNEL7);

	// после каждой передачи адрес в памяти автоматически 
	// увеличивается (чтобы перейти к следующему байту)
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL7);

	// устанавливаем размер данных периферии и памяти
	dma_set_peripheral_size(DMA1, DMA_CHANNEL7, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL7, DMA_CCR_MSIZE_8BIT);

	// высший приоритет для канала DMA
	dma_set_priority(DMA1, DMA_CHANNEL7, DMA_CCR_PL_VERY_HIGH);

	// разрешаем прерывание после завершения передачи
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);
}