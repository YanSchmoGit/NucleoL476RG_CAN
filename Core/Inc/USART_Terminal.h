//
// Created by yan on 4/4/26.
//

#ifndef NUCLEOL476RG_BMP280_USART_TERMINAL_H
#define NUCLEOL476RG_BMP280_USART_TERMINAL_H


#include <stm32l4xx.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void USART2_IRQHandler(void);
void InitUSART(uint32_t baudrate);

void SendAString(char* string);
void SendAInteger(int32_t valueInt);

#endif //NUCLEOL476RG_BMP280_USART_TERMINAL_H
