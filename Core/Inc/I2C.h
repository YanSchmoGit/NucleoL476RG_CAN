/*
 * Header file for I2C communication declarations and functions
 *
 */


#ifndef I2C_COMM_H

#define I2C_COMM_H

#include <stm32l4xx.h>
#include <stdlib.h>

// ##### Typedefs #####

typedef enum
{
    I2C_IDLE,
    I2C_READ_DATA,
    I2C_WRITE_DATA,
    I2C_WRITE_REGISTER_DATA,
    I2C_DATA_READY
} I2C_state;

typedef struct
{
    uint8_t* data;
    volatile uint8_t index;
    uint8_t numberOfBytes;
    uint8_t regAdr;
    uint8_t devAdr;
    uint8_t dataReady;
} I2C_data;

// ##### Define variables #####

extern volatile I2C_state I2CState;
extern volatile I2C_data I2CData;

// ##### Function definition #####


void InitI2C(GPIO_TypeDef* GPIOxSCL, uint8_t pinSCL, GPIO_TypeDef* GPIOxSDA, uint8_t pinSDA);

int8_t ReadI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data);

int8_t WriteI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data);


// Interrupt Handler

void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

#endif
