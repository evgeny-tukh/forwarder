#pragma once

#include "connection.h"
#include "platform_win.h"

struct SerialCfg: Config {
    uint32_t baud;
    uint8_t byteSize;
    char parity;
    uint8_t stopBits;

    SerialCfg () : baud (4800), byteSize (8), parity ('N'), stopBits (1) {}
    SerialCfg (uint32_t baudRate) : baud (baudRate), byteSize (8), parity ('N'), stopBits (1) {}
    SerialCfg (uint32_t baudRate, uint8_t bsize, char parityType, uint8_t sb) : baud (baudRate), byteSize (bsize), parity (parityType), stopBits (sb) {}
};

class SerialConnection: public Connection {
    public:
        SerialConnection (uint16_t serialPort) : Connection (Connection::Type::SERIAL, serialPort) {}

        virtual bool open ();
        virtual void close ();
        virtual uint32_t read (uint32_t size, uint8_t *buffer);
};