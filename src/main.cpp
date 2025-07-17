#include <libopencm3/stm32/rcc.h> //rcc.h - reset and clock control
#include <libopencm3/stm32/gpio.h> //inputs outputs

int main() {
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);

    while (true) {

        gpio_toggle(GPIOB, GPIO2);
        for(volatile uint32_t i =0; i < 500'000; i +=2);

    }
}