/*******************************************************************************
 * ----------------------------------------------------------------------------*
 *  elektronikaembedded@gamil.com ,https://elektronikaembedded.wordpress.com   *
 * ----------------------------------------------------------------------------*
 *                                                                             *
 * File Name  : i2c_hal.c                                                        *
 *                                                                             *
 * Description : i2c                                                           *
 *                                                                             *
 * Version     : PrototypeV1.0                                                 *
 *                                                                             *
 * --------------------------------------------------------------------------- *
 * Authors: Sarath S                                                           *
 * Date: May 16, 2017                                                          *
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
// #include "stm32f10x_rcc.h"
// #include "stm32f10x_i2c.h"
// #include "stm32f10x_gpio.h"
#include "i2c_hal.h"
#include "apds9960.h"
#include "debug.h"
#include "typedef.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* ----------------------------------------------------------------------------*
 *
 * Function Name : I2C1_init
 *
 * Description   : Stm32x I2C1_init
 *
 * Input: None
 *
 * Output  : None
 *
 * Return  : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------*
 *
 * Function Name : i2c1_read
 *
 * Description   : Read i2c1 Register Data
 *
 * Input: addr,data ,number of bytes
 *
 * Output  : data Pointer
 *
 * Return  : no of bytes wites if successfully. False otherwise
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/

 int i2c2_read(uint8_t reg, uint8_t *data, uint8_t len)
 {
     // Wait until bus is free
     while (I2C2->ISR & I2C_ISR_BUSY);
 
     // First, write the register address
     I2C2->CR2 = (APDS9960_I2C_ADDR << 1) | (1 << I2C_CR2_NBYTES_Pos)
                 | I2C_CR2_START;
     I2C2->CR2 &= ~I2C_CR2_RD_WRN;  // write mode
 
     while (!(I2C2->ISR & I2C_ISR_TXIS));
     I2C2->TXDR = reg;
 
     while (!(I2C2->ISR & I2C_ISR_TC));  // wait for transfer complete
 
     // Second, issue read request
     I2C2->CR2 = (APDS9960_I2C_ADDR << 1) | (len << I2C_CR2_NBYTES_Pos)
                 | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_AUTOEND;
 
     for (uint8_t i = 0; i < len; i++) {
         while (!(I2C2->ISR & I2C_ISR_RXNE));
         data[i] = I2C2->RXDR;
     }
 
     // Wait for STOPF
     while (!(I2C2->ISR & I2C_ISR_STOPF));
     I2C2->ICR |= I2C_ICR_STOPCF;
 
     return len;
 }
 

/* ----------------------------------------------------------------------------*
 *
 * Function Name : i2c1_write
 *
 * Description   : write date to i2c1 
 *
 * Input: addr register ,data
 *
 * Output  : None
 *
 * Return  : True if successfully write. False otherwise
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
int i2c2_write(uint8_t reg, uint8_t value)
{
    // Wait until bus is free
    while (I2C2->ISR & I2C_ISR_BUSY);

    // Setup write transfer of 2 bytes: reg + value
    I2C2->CR2 = (APDS9960_I2C_ADDR << 1) | (2 << I2C_CR2_NBYTES_Pos)
                | I2C_CR2_START | I2C_CR2_AUTOEND;

    // Wait until TXDR ready
    while (!(I2C2->ISR & I2C_ISR_TXIS));
    I2C2->TXDR = reg;

    while (!(I2C2->ISR & I2C_ISR_TXIS));
    I2C2->TXDR = value;

    // Wait for STOPF to confirm transmission is done
    while (!(I2C2->ISR & I2C_ISR_STOPF));
    I2C2->ICR |= I2C_ICR_STOPCF;

    return 1;
}
