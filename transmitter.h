#pragma once

#include <stdlib.h>
#include <string.h>
#include <cstdint>

class Transmitter {
    public:
        Transmitter (const char *remoteHost, uint16_t remotePort): connected (false), port (remotePort) {
            strcpy (host, remoteHost);
        }

        virtual bool connect () { return false; }
        virtual void disconnect () { connected = false; }
        virtual int send (char *buffer, int size) { return 0; }

    protected:
        char host [200];
        uint16_t port;
        bool connected;
};
