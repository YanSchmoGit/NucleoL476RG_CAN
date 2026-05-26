/*
* Test project for I2C functions
 * Used Hardware:
 * STM32L476RG
 * Bosch BMP280
 *
 */

#include <stm32l4xx.h>
#include <stdio.h>

#include "Init.h"
#include "I2C.h"
#include "BMP280.h"
#include "CanBus.h"

#define I2C_SCL_Pin 8
#define I2C_SCL_Port GPIOB

#define I2C_SDA_Pin 9
#define I2C_SDA_Port GPIOB

#define BMP280_DEVICE_ADR  (0x77)

#define CAN_SEND_ADR 111
#define CAN_SEND_LEN 8

uint32_t valuePress;
int32_t valueTemp;


int main(void)
{
    // TEST

    // GPIO blink init

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE5_0;


    // Init functions;
    // Init SysClock

    InitClock();


    // Init I2C
    InitI2C(I2C_SCL_Port, I2C_SCL_Pin, I2C_SDA_Port, I2C_SDA_Pin);

    // Init BMP280;
    InitBMP280(BMP280_DEVICE_ADR);


    // Init Can Bus
    CanInit();

    while (1)
    {
        // BMP280 section

        GetSensorValues(BMP280_DEVICE_ADR, &valuePress, &valueTemp);

        GPIOA->BSRR |= GPIO_BSRR_BR5;

        if (CanSend(CAN_SEND_ADR, valueTemp, valuePress,CAN_SEND_LEN) == 1)
        {
            GPIOA->BSRR |= GPIO_BSRR_BS5;
        }
    }
}
