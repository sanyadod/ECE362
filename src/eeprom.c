#include "eeprom.h"
#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>  
#include <stdlib.h>


#define EEPROM_ADDR 0x57
void enable_ports_eeprom() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1);
    GPIOA->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL9_Pos | 0xF << GPIO_AFRH_AFSEL10_Pos);
    GPIOA->AFR[1] |= (0x4 << GPIO_AFRH_AFSEL9_Pos | 0x4 << GPIO_AFRH_AFSEL10_Pos);
}

void init_i2c_eeprom() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_ANFOFF;
    I2C1->CR1 |= I2C_CR1_ERRIE;
    I2C1->CR1 |= I2C_CR1_NOSTRETCH;


    I2C1->TIMINGR = 0;
    I2C1->TIMINGR = ~I2C_TIMINGR_PRESC;
    I2C1->TIMINGR = (0x5<<28) | (0x3<<20) | (0x1<<16) | (0x3<<8) | (0x9<<0);

    I2C1->CR2 &= ~I2C_CR2_ADD10;

    I2C1->CR1 |= I2C_CR1_PE;
}

void i2c_start_prom(uint8_t targadr, uint8_t size, uint8_t dir) {
    uint32_t tmpreg = I2C1->CR2;

    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    tmpreg |= ((dir << 10) & I2C_CR2_RD_WRN);
    tmpreg |= ((targadr << 1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
    tmpreg |= I2C_CR2_START;
    I2C1->CR2 = tmpreg;
}

void i2c_stop_prom(void) {
    if ((I2C1->CR2 & I2C_CR2_STOP) != 0) return;

    I2C1->CR2 |= I2C_CR2_STOP;

    while ((I2C1->ISR & I2C_ISR_STOPF) == 0) {}

    I2C1->ICR |= I2C_ICR_STOPCF; // Clear STOPF
}

// Step 3.2: i2c_waitidle
void i2c_waitidle_prom(void) {
    while ((I2C1->ISR & I2C_ISR_BUSY) != 0) {}
}

// Step 3.3: i2c_senddata
int i2c_senddata_prom(uint8_t addr, uint8_t *data, uint8_t size) {
    i2c_waitidle_prom();
    i2c_start_prom(addr, size, 0); // Write

    for (int i = 0; i < size; i++) {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
            count++;
            if (count > 1000000) {
                return -1;
            }
            if (i2c_checknack_prom()) {
                i2c_clearnack_prom();
                i2c_stop_prom();
                return -1;
            }
        }
        I2C1->TXDR = data[i] & I2C_TXDR_TXDATA;
    }

    while (((I2C1->ISR & I2C_ISR_TC) == 0) && ((I2C1->ISR & I2C_ISR_NACKF) == 0)) {}

    if (I2C1->ISR & I2C_ISR_NACKF) {
        return -1;
    }

    i2c_stop_prom();
    return 0;
}

// Step 3.4: i2c_recvdata
int i2c_recvdata_prom(uint8_t addr, uint8_t *data, uint8_t size) {
    i2c_waitidle_prom();
    i2c_start_prom(addr, size, 1); // Read

    for (int i = 0; i < size; i++) {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
            count++;
            if (count > 1000000) return -1;
            if (i2c_checknack_prom()) {
                i2c_clearnack_prom();
                i2c_stop_prom();
                return -1;
            }
        }
        data[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }

    i2c_stop_prom();
    return 0;
}

// Step 3.5: i2c_clearnack
void i2c_clearnack_prom(void) {
    I2C1->ICR |= I2C_ICR_NACKCF;
}

// Step 3.6: i2c_checknack
int i2c_checknack_prom(void) {
    return (I2C1->ISR & I2C_ISR_NACKF) != 0;
}

// Step 4: Demonstrate data read and write for High Score
void save_high_score(uint8_t score) {
    char temp[3];
    snprintf(temp, 3, "%d", score);
    char score_byte = score;
    eeprom_write(0x0200, temp, 3);  // Store at EEPROM address 0x0000
}

uint8_t read_high_score(void) {
    char * score_byte;
    char temp[3] = {0};
    eeprom_read(0x0200, temp, 3);  // Read from EEPROM address 0x0000
    uint8_t x = strtol(temp, &score_byte, 10);
    return x;
}
/*
void save_high_score(uint8_t score) {
    char data[1] = {score};
    eeprom_write(0x0000, data, 1);
}

uint8_t read_high_score(void) {
    char data[1] = {0};
    eeprom_read(0x0000, data, 1);
    return data[0];
}*/


void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    uint8_t bytes[34];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    for(int i = 0; i<len; i++){
        bytes[i+2] = data[i];
    }
    i2c_senddata_prom(EEPROM_ADDR, bytes, len+2);
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    // ... your code here
    uint8_t bytes[2];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    i2c_senddata_prom(EEPROM_ADDR, bytes, 2);
    i2c_recvdata_prom(EEPROM_ADDR, data, len);
}
