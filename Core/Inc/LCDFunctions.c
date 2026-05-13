/*
* Source file for LCD driver
*
*/

#include "LCDFunctions.h"

// Functions


// Enable BUS for GPIO

void LCDEnableBus(GPIO_TypeDef* port)
{
    if (port == GPIOA)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    }

    if (port == GPIOB)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    }

    if (port == GPIOC)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    }

    if (port == GPIOD)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    }
}

void LCDEnableTimer()
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
}


// Timer function

void LCDWaitTime(int value)
{
    // value --> ms
    // Initialize timer

    TIM6->PSC = (16 - 1);
    TIM6->ARR = (value - 1);

    // Update flag active

    TIM6->EGR |= TIM_EGR_UG;
    TIM6->SR &= ~TIM_SR_UIF;


    // Start timer
    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;


    while (!(TIM6->SR & TIM_SR_UIF));

    TIM6->SR &= ~TIM_SR_UIF;
    TIM6->CR1 &= ~TIM_CR1_CEN;
}

// Setting up MCU Ports

void LCDSetupPortPin(GPIO_TypeDef* port, int pinNumber)
{
    port->MODER |= (1 << (pinNumber * 2));
    port->MODER &= ~(1 << ((pinNumber * 2) + 1));

    port->OTYPER &= ~(1 << pinNumber);

    port->OSPEEDR |= (1 << ((pinNumber * 2) + 1));
    port->OSPEEDR &= ~(1 << ((pinNumber * 2)));
}

void LCDSetPin(GPIO_TypeDef* port, uint8_t pinNumber, bool pinValueState)
{
    if (1 == pinValueState)
    {
        port->BSRR |= (1 << (pinNumber));
    }
    else
    {
        port->BSRR |= (1 << (pinNumber + 16));
    }
}

void LCDResetAllDataPins(bool portExpander)
{
    if (portExpander)
    {
        PortExpanderWriteOutput(PORTEXPANDER_DEVICE_ADR, PORT_A, 0b00000000);
    }
    else
    {
        LCDSetPin(LCDD0Port, LCDD0Pin, false);
        LCDSetPin(LCDD1Port, LCDD1Pin, false);
        LCDSetPin(LCDD2Port, LCDD2Pin, false);
        LCDSetPin(LCDD3Port, LCDD3Pin, false);
        LCDSetPin(LCDD4Port, LCDD4Pin, false);
        LCDSetPin(LCDD5Port, LCDD5Pin, false);
        LCDSetPin(LCDD6Port, LCDD6Pin, false);
        LCDSetPin(LCDD7Port, LCDD7Pin, false);
    }
}


void LCDSendByte(uint8_t byte, bool portExpander)
{
    if (portExpander)
    {
        PortExpanderWriteOutput(PORTEXPANDER_DEVICE_ADR, PORT_A, byte);
    }
    else
    {
        LCDSetPin(LCDD0Port, LCDD0Pin, 0b00000001 & byte);
        LCDSetPin(LCDD1Port, LCDD1Pin, 0b00000010 & byte);
        LCDSetPin(LCDD2Port, LCDD2Pin, 0b00000100 & byte);
        LCDSetPin(LCDD3Port, LCDD3Pin, 0b00001000 & byte);
        LCDSetPin(LCDD4Port, LCDD4Pin, 0b00010000 & byte);
        LCDSetPin(LCDD5Port, LCDD5Pin, 0b00100000 & byte);
        LCDSetPin(LCDD6Port, LCDD6Pin, 0b01000000 & byte);
        LCDSetPin(LCDD7Port, LCDD7Pin, 0b10000000 & byte);
    }
}

// LCD Functions


void LCDSendInstruction(uint8_t instruction)
{
    LCDSetPin(LCDEnablePort, LCDEnablePin, true);

    LCDSendByte(instruction, UsePortExpander);

    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDEnablePort, LCDEnablePin, false);
    LCDWaitTime(WaitTimeLCDCom);
    LCDResetAllDataPins(UsePortExpander);
}


void LCDSetCursorLocation(uint8_t posX, uint8_t posY)
{
    /*
     * Position on display
     *
     *      X0 X1 X2 X3 ...								 X15
     *  	0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
     * 	Y0 	x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x
     *  	64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79
     *  Y1	x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x
     *
     */


    // calculate the position on the display

    uint8_t position = posX + (64 * posY);

    LCDSetPin(LCDEnablePort, LCDEnablePin, true);
    LCDWaitTime(WaitTimeLCDCom);


    LCDSendByte(0b10000000 + position, UsePortExpander);


    LCDSetPin(LCDD7Port, LCDD7Pin, true);

    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDEnablePort, LCDEnablePin, false);
    LCDWaitTime(WaitTimeLCDCom);
    LCDResetAllDataPins(UsePortExpander);
}


void LCDSetup()
{

    // Port expander

    InitPortExpander(PORTEXPANDER_DEVICE_ADR);

    PortExpanderSetConfig(PORTEXPANDER_DEVICE_ADR, PORT_A);
    PortExpanderSetConfig(PORTEXPANDER_DEVICE_ADR, PORT_B);

    // Setup GPIO

    LCDEnableBus(GPIOC);
    LCDEnableTimer();

    LCDSetupPortPin(LCDD0Port, LCDD0Pin);
    LCDSetupPortPin(LCDD1Port, LCDD1Pin);
    LCDSetupPortPin(LCDD2Port, LCDD2Pin);
    LCDSetupPortPin(LCDD3Port, LCDD3Pin);
    LCDSetupPortPin(LCDD4Port, LCDD4Pin);
    LCDSetupPortPin(LCDD5Port, LCDD5Pin);
    LCDSetupPortPin(LCDD6Port, LCDD6Pin);
    LCDSetupPortPin(LCDD7Port, LCDD7Pin);
    LCDSetupPortPin(LCDRSPort, LCDRSPin);
    LCDSetupPortPin(LCDEnablePort, LCDEnablePin);

    // Setup LCD configuration


    LCDSendInstruction(LCDFunctionClearDisplay);
    LCDSendInstruction(LCDFunctionFunctionSet);
    LCDSendInstruction(LCDFunctionDisplayOnOffControl);
    LCDSendInstruction(LCDFunctionEntryModeSet);
}


void LCDSendCharacter(char character)
{
    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDRSPort, LCDRSPin, true);
    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDEnablePort, LCDEnablePin, true);

    LCDSendByte(character, UsePortExpander);

    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDEnablePort, LCDEnablePin, false);
    LCDWaitTime(WaitTimeLCDCom);
    LCDSetPin(LCDRSPort, LCDRSPin, false);
    LCDResetAllDataPins(UsePortExpander);
}

void LCDSendString(char* string)
{
    // As long as pointer string is not 0 stay in while-loop
    while (*string != '\0')
    {
        // Give actual value of pointer sting to "LCDSendCharacter" --> Then increase pointer to the next value!
        LCDSendCharacter(*string++);
    }
}

void LCDSendInteger(uint32_t valueInt)
{
    // Determine length of the INT value
    int lengthOfInt = snprintf(NULL, 0, "%u", valueInt);

    // Reserve pointer memory with the corresponding size
    char* numberAsString = (char*)malloc(lengthOfInt + 1);

    if (numberAsString != NULL)
    {
        // Convert INT into String
        sprintf(numberAsString, "%u", valueInt);

        LCDSendString(numberAsString);

        // Free memory space
        free(numberAsString);
    }
}

/* --> Float is disabled in the MCU settings!
	void LCDSendFloat(float_t valueFloat)
	{
		// Determine length of the INT value
		int lengthOfFloat = snprintf(NULL, 0, "%f", valueFloat);

		// Reserve pointer memory with the corresponding size
		char *numberAsString = (char *)malloc(lengthOfFloat +1 );

		if (numberAsString != NULL) {

			// Convert INT into String
		    sprintf(numberAsString, "%f", valueFloat);

		    LCDSendString(numberAsString);

		    // Free memory space
		    free(numberAsString);

		}


	}
*/
