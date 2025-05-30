#include "gesture.h"
#include "stm32f0xx.h"
#include "apds9960.h"
#include "stdlib.h"
#include "stdbool.h"
#include "i2c_hal.h"

void enable_ports_gesture() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
    GPIOA->MODER |= (GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1);
    GPIOA->AFR[1] &= ~((0xF << 12) | (0xF << 16));
    GPIOA->AFR[1] |= ((0x5 << 12) | (0x5 << 16));
}

void init_i2c_gesture() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    I2C2->CR1 &= ~I2C_CR1_PE;
    I2C2->CR1 |= I2C_CR1_ANFOFF | I2C_CR1_ERRIE;

    I2C2->TIMINGR = 0;
    I2C2->TIMINGR = (0x5<<28) | (0x3<<20) | (0x1<<16) | (0x3<<8) | (0x9<<0);

    I2C2->CR2 &= ~I2C_CR2_ADD10;
    I2C2->CR1 |= I2C_CR1_PE;
}










////extra functions

void i2c_waitidle_gesture(void) {
    while ((I2C2->ISR & I2C_ISR_BUSY) != 0) {}
}

void i2c_start_gesture(uint8_t targadr, uint8_t size, uint8_t dir) {
    uint32_t tmpreg = I2C2->CR2;
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    tmpreg |= ((dir << 10) & I2C_CR2_RD_WRN);
    tmpreg |= ((targadr << 1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
    tmpreg |= I2C_CR2_START;
    I2C2->CR2 = tmpreg;
}

void i2c_stop_gesture(void) {
    if ((I2C2->CR2 & I2C_CR2_STOP) != 0) return;
    I2C2->CR2 |= I2C_CR2_STOP;
    while ((I2C2->ISR & I2C_ISR_STOPF) == 0) {}
    I2C2->ICR |= I2C_ICR_STOPCF;
}

void i2c_clearnack_gesture(void) {
    I2C2->ICR |= I2C_ICR_NACKCF;
}

int i2c_checknack_gesture(void) {
    return (I2C2->ISR & I2C_ISR_NACKF) != 0;
}

int i2c_senddata_gesture(uint8_t addr, uint8_t *data, uint8_t size) {
    i2c_waitidle_gesture();
    i2c_start_gesture(addr, size, 0);
    for (int i = 0; i < size; i++) {
        int count = 0;
        while ((I2C2->ISR & I2C_ISR_TXIS) == 0) {
            if (++count > 1000000) return -1;
            if (i2c_checknack_gesture()) {
                i2c_clearnack_gesture();
                i2c_stop_gesture();
                return -1;
            }
        }
        I2C2->TXDR = data[i];
    }
    while (!(I2C2->ISR & I2C_ISR_TC) && (I2C2->ISR & I2C_ISR_NACKF)) {}
    if (I2C2->ISR & I2C_ISR_NACKF) return -1;
    i2c_stop_gesture();
    return 0;
}

int i2c_recvdata_gesture(uint8_t addr, uint8_t *data, uint8_t size) {
    i2c_waitidle_gesture();
    i2c_start_gesture(addr, size, 1);
    for (int i = 0; i < size; i++) {
        int count = 0;
        while ((I2C2->ISR & I2C_ISR_RXNE) == 0) {
            if (++count > 1000000) return -1;
            if (i2c_checknack_gesture()) {
                i2c_clearnack_gesture();
                i2c_stop_gesture();
                return -1;
            }
        }
        data[i] = I2C2->RXDR;
    }
    i2c_stop_gesture();
    return 0;
}

void gesture_write(uint8_t reg, const char* data, uint8_t len) {
    uint8_t buffer[33];
    buffer[0] = reg;
    for (int i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }
    i2c_senddata_gesture(APDS9960_I2C_ADDR, buffer, len + 1);
}

void gesture_read(uint8_t reg, char data[], uint8_t len) {
    i2c_senddata_gesture(APDS9960_I2C_ADDR, &reg, 1);
    i2c_recvdata_gesture(APDS9960_I2C_ADDR, (uint8_t*)data, len);
}

char detect_left_or_right_gesture() {
    char level = 0;
    char data[128];

    gesture_read(0xAE, &level, 1); // GFLVL
    if (level == 0 || level > 32) return 'N';

    gesture_read(0xFC, data, level * 4); // Read FIFO data

    int left = 0, right = 0;
    for (int i = 0; i < level; i++) {
        left  += data[i * 4 + 2];
        right += data[i * 4 + 3];
    }

    if (left - right > 100) return 'L';
    if (right - left > 100) return 'R';
    return 'N';
}
