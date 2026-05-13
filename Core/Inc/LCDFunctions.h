#ifndef LCDFunctions

#define LCDFunctions

#include <stdbool.h>

#include <stdlib.h>
#include <stm32l476xx.h>

#include "PortExpander.h"

// Definitions for LCD


#define LCDD0Pin 0
#define LCDD0Port GPIOC

#define LCDD1Pin 1
#define LCDD1Port GPIOC

#define LCDD2Pin 2
#define LCDD2Port GPIOC

#define LCDD3Pin 3
#define LCDD3Port GPIOC

#define LCDD4Pin 4
#define LCDD4Port GPIOC

#define LCDD5Pin 5
#define LCDD5Port GPIOC

#define LCDD6Pin 9
#define LCDD6Port GPIOC

#define LCDD7Pin 7
#define LCDD7Port GPIOC

#define LCDRSPin 8
#define LCDRSPort GPIOC

#define LCDEnablePin 6
#define LCDEnablePort GPIOC

#define UsePortExpander true

#define WaitTimeLCDCom ((uint16_t)1000)

// Define LCD functions

#define LCDFunctionClearDisplay 0b00000001
#define LCDFunctionReturnHome 0b00000010
#define LCDFunctionDisplayOnOffControl 0b00001110
#define LCDFunctionFunctionSet 0b00111100
#define LCDFunctionEntryModeSet 0b00000110

#define PORTEXPANDER_DEVICE_ADR  (0x20)


// Functions


// Enable BUS for GPIO

void LCDEnableBus(GPIO_TypeDef* port);


void LCDEnableTimer();


// Timer function

void LCDWaitTime(int value);


// Setting up MCU Ports

void LCDSetupPortPin(GPIO_TypeDef* port, int pinNumber);


void LCDSetPin(GPIO_TypeDef* port, uint8_t pinNumber, bool pinValueState);


void LCDResetAllDataPins(bool portExpander);


void LCDSendByte(uint8_t byte, bool portExpander);


// LCD Functions


void LCDSendInstruction(uint8_t instruction);


void LCDSetCursorLocation(uint8_t posX, uint8_t posY);


void LCDSetup();


void LCDSendCharacter(char character);


void LCDSendString(char* string);

void LCDSendInteger(uint32_t valueInt);

/* --> Float is disabled in the MCU settings!
	void LCDSendFloat(float_t valueFloat);

*/


#endif
