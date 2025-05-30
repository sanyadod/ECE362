/*******************************************************************************
 * ----------------------------------------------------------------------------*
 *  elektronikaembedded@gamil.com ,https://elektronikaembedded.wordpress.com   *
 * ----------------------------------------------------------------------------*
 *                                                                             *
 * File Name  : i2c_hal.h                                                        *
 *                                                                             *
 * Description : i2c_hal.c header file                                           *
 *                                                                             *
 * Version     : PrototypeV1.0                                                 *
 *                                                                             *
 * --------------------------------------------------------------------------- *
 * Authors: Sarath S                                                           *
 * Date: May 16, 2017                                                          *
 ******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_HAL_H
#define __I2C_HAL_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define I2C_WAIT_TIMEOUT  (uint16_t)0x5000     // Timeout for I2C operations
/* Exported functions ------------------------------------------------------- */

/* ----------------------------------------------------------------------------*
 *
 * Function Name : i2c2_init
 *
 * Description   : Stm32x i2c2_init
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
 * Function Name : i2c2_read
 *
 * Description   : Read i2c2 Register Data
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

int i2c2_read(uint8_t u8addr, uint8_t *u8data,uint8_t u8qty);


/* ----------------------------------------------------------------------------*
 *
 * Function Name : i2c2_write
 *
 * Description   : write date to i2c2 
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
int i2c2_write(uint8_t u8addr,uint8_t u8data);



#endif /* __I2C_HAL_H */

/******* https://elektronikaembedded.wordpress.com  *****END OF FILE***********/
