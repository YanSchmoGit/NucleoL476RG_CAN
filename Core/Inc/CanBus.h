//
// Created by yan on 4/20/26.
//

#ifndef NUCLEOL476RG_BMP280_CANBUS_H
#define NUCLEOL476RG_BMP280_CANBUS_H
#include <stm32l4xx.h>


// CAN Message Type Def

typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t value_1;
    uint32_t value_2;
} CANMessage;



// Functions

int8_t CanInit();

int8_t CanSend(uint32_t id, uint32_t value_1, uint32_t value_2, uint8_t len);

int8_t CanReceive(CANMessage *msg);



#endif //NUCLEOL476RG_BMP280_CANBUS_H
