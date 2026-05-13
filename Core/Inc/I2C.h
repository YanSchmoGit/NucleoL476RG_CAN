/*
 * Header file for I2C communication declarations and functions
 *
 */


#ifndef I2C

#define I2C

#include <stm32l4xx.h>
#include <stdlib.h>


// ##### Function definition #####


void InitI2C(GPIO_TypeDef* GPIOxSCL, uint8_t pinSCL, GPIO_TypeDef* GPIOxSDA, uint8_t pinSDA);

int8_t ReadI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data);

int8_t WriteI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t data);


#endif
