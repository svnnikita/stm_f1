#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "uart_setup/uart_setup.hpp"

// Тактирование портов GPIOA
void UART_Clock_Setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART2); 	
}

// Настройка режима выводов PA2 и PA3, к которым подключены линии TX и RX 
void UART_GPIO_Setup(void)
{
	// PA2 (TX) как выход с альтернативной функцией
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    // PA3 (RX) как вход с подтяжкой вверх
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_USART2_RX);

    gpio_set(GPIOA, GPIO_USART2_RX); // Включаем подтяжку вверх
}

// Настройка UART (8-N-1)
void USART2_Setup(uint32_t bod_speed, uint32_t word_size)
{
	usart_set_baudrate(USART2, bod_speed);					// скорость передачи данных
	usart_set_databits(USART2, word_size);					// размер слова
	usart_set_stopbits(USART2, USART_STOPBITS_1);			// количество стоповых битов
	usart_set_mode(USART2, USART_MODE_TX_RX);				// режим передачи данных
	usart_set_parity(USART2, USART_PARITY_NONE);			// без бита контроля
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);	// без управления потоком

	usart_enable(USART2);
}