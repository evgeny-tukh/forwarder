#pragma once 
#include <stdlib.h>
#include <cstdint>

struct Config {};

class Connection {
    public:
        enum Type {
            SERIAL,
            UDP,
            TCP,
        } type;

        typedef void *Handle;

        static const Handle INVALID_HANDLE;

        uint16_t port;
        Handle handle;
        Config *config;
        
        Connection (Type connType, uint16_t connPort) : type (connType), port (connPort) {
            handle = INVALID_HANDLE;
            config = 0;
        }

        inline bool isOpen () { return handle != INVALID_HANDLE; }

        inline void setParams (void *params) {
            config = (Config *) params;
        }

        virtual bool open () { return false; }
        virtual void close () { handle = INVALID_HANDLE; }
        virtual uint32_t read (uint32_t size, uint8_t *buffer) { return 0; }
};