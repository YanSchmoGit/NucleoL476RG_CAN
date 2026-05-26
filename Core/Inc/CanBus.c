//
// Created by yan on 4/20/26.
//

#include "CanBus.h"


int8_t CanInit()
{
    // CAN Configuration

    // Enable GPIO A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    // Enable CAN
    RCC->APB1ENR1 |= RCC_APB1ENR1_CAN1EN;

    // Alternate function for PA 11 & PA 12 - AF9

    GPIOA->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12);
    GPIOA->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL11_Msk | GPIO_AFRH_AFSEL12_Msk);
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL11_0 | GPIO_AFRH_AFSEL11_3 | GPIO_AFRH_AFSEL12_0 | GPIO_AFRH_AFSEL12_3;


    // Init Can

    // Enter CAN initialization modus - necessary to change CAN settings
    CAN1->MCR |= CAN_MCR_INRQ;
    while (!(CAN1->MSR & CAN_MSR_INAK))
    {
    }
    CAN1->MCR &= ~CAN_MCR_SLEEP;

    // Set Baudrate - 500 kbit/s at 16 MHz MCU system clock
    // Baudrate = (F_Sys / (Prescaler * (1 + TS1 + TS2)))

    CAN1->BTR = (1 << CAN_BTR_BRP_Pos) // Prescaler (value 2)
        | (9 << CAN_BTR_TS1_Pos) // TS1 (value 13)
        | (4 << CAN_BTR_TS2_Pos) // TS2 (value 2)
        | (0 << CAN_BTR_SJW_Pos); // Resynchronization jump width (value 1)

    // Switch CAN mode - Normal mode

    CAN1->MCR &= ~CAN_MCR_INRQ; // Normal mode
    while (CAN1->MSR & CAN_MSR_INAK)
    {
    } // Wait for mode switch


    return 1;
}

int8_t CanFilter(uint16_t id, uint16_t mask)
{
    // Configure filter

    CAN1->FMR |= CAN_FMR_FINIT; // Init mode for all filter
    CAN1->FA1R &= ~CAN_FA1R_FACT0; // Deactivate filter 0
    CAN1->FS1R |= CAN_FS1R_FSC0; // Enable 32-bit scale mode
    CAN1->FM1R &= ~CAN_FM1R_FBM0; // Identifier mask mode

    CAN1->sFilterRegister[0].FR1 = id << 21; // Filter id  - shift 21 bytes --> Standard ID is used (Bits 21:31)
    CAN1->sFilterRegister[0].FR2 = mask << 21; // Filter mask - shift 21 bytes --> Standard ID is used (Bits 21:31)

    CAN1->FFA1R &= ~(CAN_FFA1R_FFA0); // Assign filter 0 to FIFO 0
    CAN1->FA1R |= CAN_FA1R_FACT0;
    CAN1->FMR &= ~CAN_FMR_FINIT;

    return 1;
}

// Send function


int8_t CanSend(uint32_t id, uint32_t value_1, uint32_t value_2, uint8_t len)
{
    if (CAN1->TSR & CAN_TSR_TME0) // Check if Mailbox 0 is empty
    {
        CAN1->sTxMailBox[0].TIR = (id << CAN_TI0R_STID_Pos); // Set standard ID
        CAN1->sTxMailBox[0].TDTR = (len & CAN_TDT0R_DLC_Msk); // Set data length

        // Write frist 2 data words to mailbox low
        CAN1->sTxMailBox[0].TDLR = value_1;

        // Write second 2 data words to mailbox high
        CAN1->sTxMailBox[0].TDHR = value_2;

        CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ; //Send request

        return 1;
    }
    else
        return 0;
}

// Can Receive

int8_t CanReceive(CANMessage* msg)
{
    // Check if message s ready

    if ((CAN1->RF0R & CAN_RF0R_FMP0) == 0)
    {
        return 0; // No message received
    }

    // Extract ID from data
    msg->id = (CAN1->sFIFOMailBox[0].RIR >> CAN_RI0R_STID_Pos);

    // Get data length
    msg->dlc = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC_Msk);

    // Get data from data registers

    uint32_t lowReg = CAN1->sFIFOMailBox[0].RDLR;
    uint32_t highReg = CAN1->sFIFOMailBox[0].RDHR;

    // Divide data to output structure

    msg->value_1 = lowReg;
    msg->value_2 = highReg;

    // Release pending message
    CAN1->RF0R |= CAN_RF0R_RFOM0;

    return 1; // Message received
}
