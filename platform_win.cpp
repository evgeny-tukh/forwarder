#include "platform_win.h"

static const uint8_t XON = 0x11 ;
static const uint8_t XOFF = 0x13;

char *getComPortName (char *buffer, uint16_t port) {
    if (buffer)
        sprintf (buffer, "\\\\.\\COM%d", port);
    
    return buffer;
}

Connection::Handle openComPort (uint16_t port) {
    char buffer [100];

    return (Connection::Handle) CreateFile (getComPortName (buffer, port), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
}

void configureComPort (Connection::Handle handle, SerialCfg *config) {
    DCB settings;
    COMMTIMEOUTS timeouts;

    memset (& settings, 0, sizeof (settings));

    timeouts.ReadIntervalTimeout = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 3000;

    GetCommState ((HANDLE) handle, & settings);

    settings.fBinary = 1;
    settings.fParity = 1;
    settings.fInX = 1;
    settings.XoffChar = XOFF;
    settings.XonChar = XON;
    settings.BaudRate = config->baud;
    settings.ByteSize = config->byteSize;
    settings.StopBits = ONESTOPBIT;

    switch (config->parity) {
        case 'N': settings.Parity = NOPARITY; break;
        case 'O': settings.Parity = ODDPARITY; break;
        case 'E': settings.Parity = EVENPARITY; break;
        case 'M': settings.Parity = MARKPARITY; break;
        case 'S': settings.Parity = SPACEPARITY; break;
        default: settings.Parity = NOPARITY;
    }

    SetCommState ((HANDLE) handle, & settings);
    SetCommTimeouts ((HANDLE) handle, & timeouts);
    //EscapeCommFunction ((HANDLE) handle, SETDTR);
}

void closeComPort (Connection::Handle handle) {
    CloseHandle ((HANDLE) handle);
}

uint32_t readComPort (Connection::Handle handle, uint8_t *buffer, uint32_t size) {
    unsigned long bytesRead;

    return ReadFile ((HANDLE) handle, buffer, size, & bytesRead, 0) ? (uint32_t) bytesRead : 0;
}

uint32_t getErrorCode () {
    return GetLastError ();
}

WinTransmitter::WinTransmitter (const char *remoteHost, uint16_t remotePort): Transmitter (remoteHost, remotePort) {
    handle = INVALID_SOCKET;
}

bool WinTransmitter::connect () {
    SOCKADDR_IN remoteHost;

    handle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    remoteHost.sin_addr.S_un.S_addr = inet_addr (host);
    remoteHost.sin_family = AF_INET;
    remoteHost.sin_port = ntohs (port);

    return ::connect (handle, (SOCKADDR *) & remoteHost, sizeof (remoteHost)) == S_OK;
}

void WinTransmitter::disconnect () {
    closesocket (handle);

    handle = INVALID_SOCKET;
}

int WinTransmitter::send (char *buffer, int size) {
    return ::send (handle, buffer, size, 0);
}

Transmitter *createTransmitter (const char *remoteHost, uint16_t remotePort) {
    return new WinTransmitter (remoteHost, remotePort);
}