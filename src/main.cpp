#include <libopencm3/stm32/rcc.h> 	// rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> 	// inputs outputs
#include <libopencm3/stm32/timer.h>

#define PRESC 8000
#define PERIOD 1000
#define LIGHT_TIME_VALUE 100

// ТАКТИРОВАНИЕ
static void clock_setup(void)
{
	// Разгоняем МК до 64 МГц
	//rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);

	// Тактирование таймера TIM1
	rcc_periph_clock_enable(RCC_TIM2);

	// Тактирование портов GPIOB
	rcc_periph_clock_enable(RCC_GPIOB);
}


// Настройка режима вывода PB2, к которому подключен светодиод
static void gpio_setup(void)
{	
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
					GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
}


// Настройка таймера
static void timer_setup(void) {
	// Устанавливаем предделитель тактовой частоты.
	// Значение счетчика увеличивается на единицу каждую милисекунду
	timer_set_prescaler(TIM2, PRESC - 1);

	// Период
	// Счетчик считает до значения PERIOD - 1
	timer_set_period(TIM2, PERIOD - 1);
	
	// Включение счетчика
	timer_enable_counter(TIM2);
}


int main() {
	clock_setup();
	gpio_setup();
	timer_setup();
	
	while(true){
		// При изменении значения LIGHT_TIME_VALUE изменяется длительность свечения светодиода.
		// Например, при значении 200 светодиод горит до достижения счетчиком
		// этого значения, а остальное время (PERIOD - 200 = 800) светодиод погашен.  
		if (timer_get_counter(TIM2) < LIGHT_TIME_VALUE) {
			gpio_set(GPIOB, GPIO2);
		}
		else {
			gpio_clear(GPIOB, GPIO2);
		}
	}
}