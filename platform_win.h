#pragma once

#include <stdio.h>
#include <cstdint>
#include <windows.h>
#include "connection.h"
#include "serial.h"

extern char *getComPortName (char *buffer, uint16_t port);
Connection::Handle openComPort (uint16_t port);
void configureComPort (Connection::Handle handle, struct SerialCfg *config);
void closeComPort (Connection::Handle handle);
uint32_t readComPort (Connection::Handle handle, uint8_t *buffer, uint32_t size);