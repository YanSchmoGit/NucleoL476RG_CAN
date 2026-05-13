/*
 * Source file for I2C communication and functions
 *
 */


#include "I2C.h"


void EnableBusClock(GPIO_TypeDef* GPIOx)
{
    if (GPIOx == GPIOA)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    }

    if (GPIOx == GPIOB)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    }

    if (GPIOx == GPIOC)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    }

    if (GPIOx == GPIOD)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    }
}


void InitI2C(GPIO_TypeDef* GPIOxSCL, uint8_t pinSCL, GPIO_TypeDef* GPIOxSDA, uint8_t pinSDA)
{
    // ##### Activate bus clocks #####

    // GPIO
    EnableBusClock(GPIOxSCL);
    EnableBusClock(GPIOxSDA);

    // I2C
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;


    // ##### GPIO register settings #####

    // PUPG & Outputtype

    GPIOxSCL->PUPDR &= ~(0x3UL << (pinSCL * 2));
    GPIOxSDA->PUPDR &= ~(0x3UL << (pinSDA * 2));

    GPIOxSCL->OTYPER |= (0x1UL << pinSCL);
    GPIOxSDA->OTYPER |= (0x1UL << pinSDA);

    // MODER & AF register

    GPIOxSCL->MODER &= ~(0x3UL << (pinSCL * 2));
    GPIOxSCL->MODER |= (0x2UL << (pinSCL * 2));


    if (pinSCL < 8)
    {
        GPIOxSCL->AFR[0] &= ~(0xFUL << (pinSCL * 4));
        GPIOxSCL->AFR[0] |= (0x4UL << (pinSCL * 4)); // AF 4
    }
    else
    {
        GPIOxSCL->AFR[1] &= ~(0xFUL << ((pinSCL - 8) * 4));
        GPIOxSCL->AFR[1] |= (0x4UL << ((pinSCL - 8) * 4)); // AF 4
    }

    GPIOxSDA->MODER &= ~(0x3UL << (pinSDA * 2));
    GPIOxSDA->MODER |= (0x2UL << (pinSDA * 2));

    if (pinSCL < 8)
    {
        GPIOxSDA->AFR[0] &= ~(0xFUL << (pinSDA * 4));
        GPIOxSDA->AFR[0] |= (0x4UL << (pinSDA * 4)); // AF 4
    }
    else
    {
        GPIOxSCL->AFR[1] &= ~(0xFUL << ((pinSDA - 8) * 4));
        GPIOxSDA->AFR[1] |= (0x4UL << ((pinSDA - 8) * 4)); // AF 4
    }


    // ##### I2C initialisieren #####

    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->TIMINGR = 0x00503D58; //  Clock 16mHz
    I2C1->CR1 |= I2C_CR1_PE;
}

int8_t ReadI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data)
{
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Configure control register (setting device address, number of bytes, etc...)
    I2C1->CR2 = ((devAdr << 1) | (1 << I2C_CR2_NBYTES_Pos) | I2C_CR2_START);

    while (I2C1->CR2 & I2C_CR2_START);

    I2C1->TXDR = regAdr;
    while (!(I2C1->ISR & I2C_ISR_TXE));

    // Change mode to Read and get Data


    I2C1->CR2 = ((devAdr << 1) | (numberOfBytes << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND |
        I2C_CR2_START);

    while (I2C1->CR2 & I2C_CR2_START);


    for (size_t i = 0; i < numberOfBytes; i++)
    {
        while (!(I2C1->ISR & I2C_ISR_RXNE));
        *data = I2C1->RXDR;

        data++;
    }

    // Stop transmission
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR |= I2C_ICR_STOPCF;


    return 0;
}

int8_t WriteI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t data)
{
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Configure control register (setting device address, number of bytes, etc...)
    I2C1->CR2 = (devAdr << 1) | (numberOfBytes << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START;

    while (I2C1->CR2 & I2C_CR2_START);

    // Set register
    I2C1->TXDR = regAdr;
    while (!(I2C1->ISR & I2C_ISR_TXE));

    // Set data
    I2C1->TXDR = data;
    while (!(I2C1->ISR & I2C_ISR_TXE));

    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR |= I2C_ICR_STOPCF;

    return 0;
}
