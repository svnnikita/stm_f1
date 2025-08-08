#include <libopencm3/stm32/rcc.h> 	// rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> 	// inputs outputs
#include <libopencm3/stm32/usart.h>

#define SPEED 115200
#define WSIZE 8

// ТАКТИРОВАНИЕ
static void clock_setup(void)
{
	//rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);	// Разгоняем МК до 64 МГц

	rcc_periph_clock_enable(RCC_GPIOB);

	rcc_periph_clock_enable(RCC_TIM2);		// Тактирование таймера TIM1
	rcc_periph_clock_enable(RCC_GPIOA);		// Тактирование портов GPIOB
	rcc_periph_clock_enable(RCC_USART2); 	
}


// Настройка режима выводов PA2 и PA3, к которым подключены линии TX и RX 
static void gpio_setup(void)
{	
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
					GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
}

// Настройка UART (8-N-1)
void usart2_setup(void)
{
	/* Setup UART parameters. */
	usart_set_baudrate(USART2, SPEED);						// скорость передачи данных
	usart_set_databits(USART2, WSIZE);						// размер слова
	usart_set_stopbits(USART2, USART_STOPBITS_1);			// количество стоповых битов
	usart_set_mode(USART2, USART_MODE_TX);					// режим передачи данных
	usart_set_parity(USART2, USART_PARITY_NONE);			// без бита контроля
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);	// без управления потоком

	/* Finally enable the USART. */
	usart_enable(USART2);
}


int main() {
	clock_setup();
	gpio_setup();
	usart2_setup();
	
	// i -- используется для формирования задержки
	// j -- счетчик, при помощи которого происходит перемещение на новую строку
	// c -- используется для формирования символов
	uint32_t i;
	uint16_t j = 0, c = 0;

	/* Blink the LED (PB2) on the board with every transmitted byte. */
	while (1) {
		gpio_toggle(GPIOB, GPIO2);				/* LED on/off */
		usart_send_blocking(USART2, c + '0');	/* преобразуем число в символ */

		c = (c == 9) ? 0 : c + 1;				/* Increment c. */
		
		// if ((j++ % 80) == 0) {					/* Newline after line full. */
		// 	usart_send_blocking(USART2, '\r');
		// 	usart_send_blocking(USART2, '\n');
		// }
		
		for (i = 0; i < 4000000; i++)			/* Wait a bit. */
			__asm__("nop");
	}

}