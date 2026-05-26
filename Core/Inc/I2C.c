/*
 * Source file for I2C communication and functions
 *
 */


#include "I2C.h"

volatile I2C_state I2CState = I2C_IDLE;
volatile I2C_data I2CData;


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

    if (pinSDA < 8)
    {
        GPIOxSDA->AFR[0] &= ~(0xFUL << (pinSDA * 4));
        GPIOxSDA->AFR[0] |= (0x4UL << (pinSDA * 4)); // AF 4
    }
    else
    {
        GPIOxSDA->AFR[1] &= ~(0xFUL << ((pinSDA - 8) * 4));
        GPIOxSDA->AFR[1] |= (0x4UL << ((pinSDA - 8) * 4)); // AF 4
    }


    // ##### I2C initialisieren #####

    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->TIMINGR = 0x00503D58; //  Clock 16mHz

    // Enable interrupts

    I2C1->CR1 |= I2C_CR1_TXIE | // Transmit interrupt enable
        I2C_CR1_RXIE | // Receive interrupt enable
        I2C_CR1_TCIE | // Transfer complete interrupt enable
        I2C_CR1_ERRIE | // Error interrupt enable
        I2C_CR1_STOPIE; // Stop interrupt enable


    I2C1->CR1 |= I2C_CR1_PE;

    // Enable NVIC interrupts

    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
}

int8_t ReadI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data)
{
    while (I2CState != I2C_IDLE)
    {
    }


    // Write data to status structure
    I2CData.devAdr = devAdr;
    I2CData.regAdr = regAdr;
    I2CData.numberOfBytes = numberOfBytes;
    I2CData.data = data; // Pointer to data
    I2CData.index= 0;

    // Set state
    I2CState = I2C_WRITE_REGISTER_DATA;

    // Configure control register (setting device address, number of bytes, etc...)
    uint32_t cr2 = I2C1->CR2;
    cr2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    cr2 |= ((devAdr << 1) | (1 << I2C_CR2_NBYTES_Pos) | I2C_CR2_START);
    I2C1->CR2 = cr2;


    return 0;
}

int8_t WriteI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data)
{
   while (I2CState != I2C_IDLE)
    {
    }

    // Write data to status structure
    I2CData.devAdr = devAdr;
    I2CData.regAdr = regAdr;
    I2CData.numberOfBytes = numberOfBytes;
    I2CData.data = data; // Pointer to data

    // Set state
    I2CState = I2C_WRITE_REGISTER_DATA;

    // Configure control register (setting device address, number of bytes, etc...)
    uint32_t cr2 = I2C1->CR2;
    cr2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    cr2 |= (devAdr << 1) | ((numberOfBytes) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START;
    I2C1->CR2 = cr2;


    return 0;
}

void I2C1_EV_IRQHandler(void)
{
    // Buffer empty send register address
    if (I2C1->ISR & I2C_ISR_TXIS)
    {

        if (I2CState == I2C_WRITE_REGISTER_DATA)
        {
            uint8_t current_nbytes = (uint8_t)((I2C1->CR2 & I2C_CR2_NBYTES_Msk) >> I2C_CR2_NBYTES_Pos);

            if (current_nbytes > 1)
            {
                // Set state
                I2CState = I2C_WRITE_DATA;
            }

            I2C1->TXDR = I2CData.regAdr;
        }
        else if (I2CState == I2C_WRITE_DATA)
        {
            I2C1->TXDR = *I2CData.data;
        }
    }


    // Register address send finish
    if ((I2C1->ISR & I2C_ISR_TC) && (I2CState == I2C_WRITE_REGISTER_DATA))
    {
        // Set state

        I2CState = I2C_READ_DATA;

        // Reset Cr2 Register
        uint32_t cr2 = I2C1->CR2;
        cr2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);

        // Set I2C to receive, generate new start condition
        cr2 |= (I2CData.devAdr << 1);
        cr2 |= I2C_CR2_RD_WRN;
        cr2 |= (I2CData.numberOfBytes << I2C_CR2_NBYTES_Pos);
        cr2 |= I2C_CR2_AUTOEND;
        cr2 |= I2C_CR2_START;

        I2C1->CR2 = cr2;
    }

    // Receive data
    if (I2C1->ISR & I2C_ISR_RXNE)
    {
        *I2CData.data = I2C1->RXDR;
        I2CData.data++;
        I2CData.index++;
/*
        if (I2CData.index >= I2CData.numberOfBytes)
        {
            I2C1->CR1 &= ~I2C_CR1_RXIE;
        }*/
    }


    // Transfer finished

    if (I2C1->ISR & I2C_ISR_STOPF)
    {
        I2C1->ICR = I2C_ICR_STOPCF; // Reset stop flag

        if (I2CState == I2C_READ_DATA)
        {
            // Set status
            I2CState = I2C_DATA_READY;
        }
        else
        {
            I2CState = I2C_IDLE;
        }
    }
}


void I2C1_ER_IRQHandler(void)
{
    uint32_t errorStatus;
    errorStatus = I2C1->ISR;

    if (I2C1->ISR & I2C_ISR_NACKF)
    {
        I2C1->ICR = I2C_ICR_NACKCF; // Reset NACK-Flag
    }

    if (I2C1->ISR & I2C_ISR_ARLO)
    {
        I2C1->ICR = I2C_ICR_ARLOCF;
        ;
    }
    I2C1->CR2 |= I2C_CR2_STOP; // Stop Communication
    I2CState = I2C_IDLE;
}
