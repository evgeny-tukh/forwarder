#pragma once

#include <stdio.h>
#include <cstdint>
#include <windows.h>
#include "connection.h"
#include "serial.h"
#include "transmitter.h"

extern char *getComPortName (char *buffer, uint16_t port);
Connection::Handle openComPort (uint16_t port);
void configureComPort (Connection::Handle handle, struct SerialCfg *config);
void closeComPort (Connection::Handle handle);
uint32_t readComPort (Connection::Handle handle, uint8_t *buffer, uint32_t size);
uint32_t getErrorCode ();

class WinTransmitter: public Transmitter {
    public:
        WinTransmitter (const char *remoteHost, uint16_t remotePort);

        virtual bool connect ();
        virtual void disconnect ();
        virtual int send (char *buffer, int size);

    private:
        SOCKET handle;
};

Transmitter *createTransmitter (const char *remoteHost, uint16_t remotePort);
