#include "sound.h"
#include "stm32f0xx.h"
#include <stdint.h>
#include <stdbool.h>

void dac_init(void) {
    // Enable DAC and GPIOA clock

    // Enable DAC channel 1
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // Set PA4 to Analog mode
    GPIOA->MODER &= ~(3 << (4*2));   // Clear MODER4
    GPIOA->MODER |=  (3 << (4*2));   // Set MODER4 to Analog

    // Disable trigger and enable DAC channel 1
    DAC->CR &= ~DAC_CR_TEN1;   // Disable trigger
    DAC->CR |=  DAC_CR_EN1;    // Enable DAC
}

void dac_output(uint16_t val) {
    // Write value (12-bit right-aligned)
    DAC->DHR12R1 = val & 0x0FFF;
}

void delay_ms(uint32_t ms) {
    ms *= 8000; // Assuming 8 MHz core clock
    while(ms--);
}
// void init_buzzer() {
//     RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
//     GPIOA->MODER &= ~(3 << (4 * 2));    // Clear PA4 mode
//     GPIOA->MODER |=  (1 << (4 * 2));    // Set PA4 to output
//     GPIOA->OTYPER &= ~(1 << 4);         // Push-pull (default)
//     GPIOA->OSPEEDR &= ~(3 << (4 * 2));  // Low speed (default)
// }


void buzz_for_sound(int delay) {
    for (int i = 0; i < 100; i++) {       // Buzz 100 times
        GPIOA->BSRR = GPIO_BSRR_BS_4;      // PA4 HIGH
        for (volatile int j = 0; j < delay; j++);
        GPIOA->BSRR = GPIO_BSRR_BR_4;      // PA4 LOW
        for (volatile int j = 0; j < delay; j++);
    }
}

void init_buzzer() {
    // Enable clock to GPIOA and DAC
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // Set PA4 to analog mode (DAC output)
    GPIOA->MODER &= ~(3 << (4 * 2)); // Clear mode
    GPIOA->MODER |= (3 << (4 * 2));  // Set to analog mode

    // Enable DAC Channel 1
    DAC->CR |= DAC_CR_EN1;
}

void buzz_with_dac(int step_delay, int buzz_time_ms) {
    int steps = (buzz_time_ms * 1000) / (step_delay * 100); // rough estimate
    for (int count = 0; count < steps; count++) {
        for (int i = 0; i < 50; i++) {
            DAC->DHR12R1 = i * 80;
            for (volatile int d = 0; d < step_delay; d++);
        }
        for (int i = 50; i >= 0; i--) {
            DAC->DHR12R1 = i * 80;
            for (volatile int d = 0; d < step_delay; d++);
        }
    }
    DAC->DHR12R1 = 0; // Turn off sound after buzzing
}