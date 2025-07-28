#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static void clock_setup(void)
{
	/* Set STM32 to 64 MHz. */
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);

	/* Enable alternate function peripheral clock. */
	//rcc_periph_clock_enable(RCC_AFIO);

	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
}

static void gpio_setup(void)
{
	
	/* Set GPIO4 and 5 (in GPIO port B) to 'output push-pull'. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
	
}

int main(void)
{
	volatile int i;

	clock_setup();
	gpio_setup();

	/* Blink the LEDs on the board. */
	while (1) {
		gpio_toggle(GPIOB, GPIO2);	/* LED on/off */
		for (i = 0; i < 4000000; i++) /* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}