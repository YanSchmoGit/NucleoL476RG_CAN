/*
 * Source file for Init functions
 *
 *
 *
 */

#include "Init.h"


void InitClock()
{
    // Set Clock

    RCC->CR |= RCC_CR_HSION;

    while (!(RCC->CR & RCC_CR_HSIRDY));

    // Configure waite states
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0WS;

    // Select HSI as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    // Wait until clock is selected
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    /*
    // MSI Bereich auf 8 MHz einstellen (Range 7)
    RCC->CR &= ~RCC_CR_MSIRANGE;
    RCC->CR |= RCC_CR_MSIRANGE_7; // 0111 = 8 MHz

    // MSI Range Auswahl über CR Register aktivieren
    RCC->CR |= RCC_CR_MSIRGSEL;

    // MSI einschalten und warten
    RCC->CR |= RCC_CR_MSION;
    while(!(RCC->CR & RCC_CR_MSIRDY));

    // System Clock auf MSI umschalten
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_MSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);
*/
}
