#include "serial.h"

bool SerialConnection::open () {
    handle = openComPort (port);

    if (handle != Connection::INVALID_HANDLE && config) {
        configureComPort (handle, (SerialCfg *) config);
    }

    return handle != Connection::INVALID_HANDLE;
}

void SerialConnection::close () {
    if (handle != Connection::INVALID_HANDLE) {
        closeComPort (handle);
        
        handle = Connection::INVALID_HANDLE;
    }
}

uint32_t SerialConnection::read (uint32_t size, uint8_t *buffer) {
    return readComPort (handle, buffer, size);
}
