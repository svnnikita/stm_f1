#include <libopencm3/stm32/rcc.h> 	// rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> 	// inputs outputs
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define PRESC 8000	// предделитель частоты работы таймера
#define PERIOD 500	// значение, до которого считает счетчик

// ТАКТИРОВАНИЕ
static void clock_setup(void)
{
	//rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);	// Разгоняем МК до 64 МГц
	rcc_periph_clock_enable(RCC_TIM2);	// Тактирование таймера TIM1
	rcc_periph_clock_enable(RCC_GPIOB);	// Тактирование портов GPIOB
}


// Настройка режима вывода PB2, к которому подключен светодиод
static void gpio_setup(void)
{	
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
					GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
	
	gpio_clear(GPIOB, GPIO2);
}


// Настройка таймера
static void timer_setup(void) 
{
	// Устанавливаем предделитель тактовой частоты.
	// Значение счетчика увеличивается на единицу каждую милисекунду
	timer_set_prescaler(TIM2, PRESC - 1);

	// Период
	// Счетчик считает до значения PERIOD - 1
	timer_set_period(TIM2, PERIOD - 1);
	
	// Включение счетчика
	timer_enable_counter(TIM2);
}

// Настройка прерываний
void interrupt_setup(void)
{
	timer_enable_irq(TIM2, TIM_DIER_UIE);	// разрешаем прерывания
	nvic_enable_irq(NVIC_TIM2_IRQ);
}

// Обработчик прерывания
void tim2_isr(void)
{ 
	timer_clear_flag(TIM2, TIM_SR_UIF);
	gpio_toggle(GPIOB,GPIO2);
}

int main() {
	clock_setup();
	gpio_setup();
	timer_setup();
	interrupt_setup();
	
	while(true){
	}
}