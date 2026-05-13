//
// Created by yan on 4/4/26.
//

#include "USART_Terminal.h"


/* USART communication
     *
     *
     * */
volatile char* outString = "";

bool workInProgress = false;

void InitUSART(uint32_t baudrate)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // PA2 config
    GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE2_1;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL2;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2;

    // PA3 config
    GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL3;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2;

    // Enable USART2

    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    USART2->BRR = 16000000 / baudrate; //0x23; // Baudrate 115200 Bps; // Systemclock / Baurate

    USART2->CR1 &= ~(USART_CR1_M0_Msk | USART_CR1_M1_Msk); // Set word length: 1 start bit, 8 data bits, n stop bits
    USART2->CR1 &= ~USART_CR1_PCE_Msk; // Disable parity control

    USART2->CR1 |= USART_CR1_UE; // Start USART2
    USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Enable receiver & transmitter

    //USART2->CR1 |= USART_CR1_RXNEIE; // Enable receive interrupt

    NVIC_EnableIRQ(USART2_IRQn);
}

void SendAString(char* string)
{
    if (!workInProgress)
    {
        outString = string;
        USART2->CR1 |= USART_CR1_TXEIE;
        USART2->TDR = *outString++;
        workInProgress = true;
    }
}

void SendAInteger(int32_t valueInt)
{
    int lengthOfInt = snprintf(NULL, 0, "%ld", valueInt);

    // Reserve pointer memory with the corresponding size
    char* numberAsString = (char*)malloc(lengthOfInt + 1);

    if (numberAsString != NULL)
    {
        // Convert INT into String
        sprintf(numberAsString, "%ld\r\n", valueInt);

        SendAString(numberAsString);

        // Free memory space
        free(numberAsString);
    }
}

void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_TXE)
    {
        if (*outString != '\0')
        {
            USART2->TDR = *outString++;
        }
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;
            workInProgress = false;
        }
    }
}
