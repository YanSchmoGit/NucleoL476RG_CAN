//
// Created by yan on 4/7/26.
//

#include "PortExpander.h"


int8_t InitPortExpander(uint8_t devAdr)
{
    // Set configuration of chip

    // IOCON Port A

    WriteI2C(devAdr, MCP23017_REGISTER_IOCONA, 2, MCP23017_SEQOP_OFF);

    // IOCON Port A

    WriteI2C(devAdr, MCP23017_REGISTER_IOCONB, 2, MCP23017_SEQOP_OFF);

    return 0;
};


int8_t PortExpanderSetConfig(uint8_t devAdr, PortDef port)
{
    if (port == PORT_A)
    {
        // Configure all ports as output
        WriteI2C(devAdr, MCP23017_REGISTER_IODIRA, 2, MCP23017_IODIR_ALL_OUTPUT);
    }

    if (port == PORT_B)
    {
        // Configure all ports as output
        WriteI2C(devAdr, MCP23017_REGISTER_IODIRB, 2, MCP23017_IODIR_ALL_OUTPUT);
    }

    return 0;
};

int8_t PortExpanderWriteOutput(uint8_t devAdr, PortDef port, uint8_t value)
{
    if (port == PORT_A)
    {
        WriteI2C(devAdr, MCP23017_REGISTER_GPIOA, 2, value);
    }

    if (port == PORT_B)
    {
        WriteI2C(devAdr, MCP23017_REGISTER_GPIOB, 2, value);
    }

    return 0;
};
