#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <windows.h>
#include <string>
#include "connection.h"
#include "serial.h"

struct Cfg {
    Connection::Type connType;
    uint32_t port, remotePort, baud;
    std::string dataHost, remoteHost, bindHost;

    Cfg (): connType (Connection::Type::SERIAL), port (0), remotePort (0), baud (4800) {
        remoteHost = "jecat.1gb.ru";
    }
};

void showHelp () {
    printf (
        "USAGE:\n\n"
        "\t-h\t\tshows help\n"
        "\t-c:{s|u|t}\tselects connection type (serial/UDP/TCP)\n"
        "\t-p:nnnn\t\tselects port (for serial no COM needed)\n"
        "\t-b:nnnn\t\tselects baud rate (for serial connection)\n"
        "\t-n:host\t\tselects NIC to bind (for UDP and TCP)\n"
        "\t-t:host\t\tselects host to connect to data source (for TCP)\n"
        "\t-r:host:port\tselects remote host and port to transfer\n"
    );
}

void parseArgs (int argCount, char *args [], Cfg& cfg) {
    if (argCount < 2) {
        showHelp ();
        exit (0);
    }

    for (auto i = 1; i < argCount; ++ i) {
        char *arg = args [i];

        if (*arg != '-' && *arg != '/') {
            showHelp ();
            exit (0);
        }

        switch (tolower (arg [1])) {
            case 'h':
                showHelp ();
                exit (0);

            case 'c':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                switch (arg [3]) {
                    case 's': cfg.connType = Connection::Type::SERIAL; break;
                    case 'u': cfg.connType = Connection::Type::UDP; break;
                    case 't': cfg.connType = Connection::Type::TCP; break;
                }

                break;

            case 'b':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                cfg.baud = atoi (arg + 3); break;

            case 'p':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                cfg.port = atoi (arg + 3); break;

            case 'n':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                cfg.bindHost = arg + 3; break;

            case 't':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                cfg.dataHost = arg + 3; break;

            case 'r':
                if (arg [2] != ':') {
                    showHelp ();
                    exit (0);
                }

                char *colon = strchr (arg + 3, ':');

                if (colon) {
                    cfg.remoteHost.clear ();
                    cfg.remoteHost.append (arg + 3, colon);
                    
                    cfg.remotePort = atoi (colon + 1);
                } else {
                    cfg.remotePort = atoi (arg + 3);
                }

                break;
        }
    }
}

void showSettings (Cfg& cfg) {
    if (!cfg.port) {
        printf ("No port scpecified.\n");
        exit (0);
    }

    if (!cfg.remotePort) {
        printf ("No remote port scpecified.\n");
        exit (0);
    }

    switch (cfg.connType) {
        case Connection::Type::SERIAL:
            printf ("Serial connection (port COM%d, baud rate %d)\n", cfg.port, cfg.baud); break;

        case Connection::Type::UDP:
            printf ("UDP connection (port %d)\n", cfg.port);

            break;

        case Connection::Type::TCP:
            printf ("TCP connection (port %d)\n", cfg.port);
            
            break;
    }

    printf ("Forwarding data to %s:%d\n", cfg.remoteHost.c_str (), cfg.remotePort);
}

int main (int argCount, char *args []) {
    Cfg cfg;
    Connection *connection;
    Config *connCfg;

    printf ("Forwader tool v1.0\nCopyright (c) by Evgeny Tukh, 2020\n");

    parseArgs (argCount, args, cfg);

    showSettings (cfg);

    printf ("Opening connection...");

    switch (cfg.connType) {
        case Connection::Type::SERIAL:
            connection = new SerialConnection (cfg.port);
            connCfg = new SerialCfg (cfg.baud);

            break;

        default:
            printf ("Unsupported connection type.\n");
            exit (0);
    }

    connection->setParams (connCfg);

    WSAData data;

    WSAStartup (0x0202, & data);

    auto transmitter = createTransmitter (cfg.remoteHost.c_str (), cfg.remotePort);

    printf ("Connecting to remote host...");

    if (transmitter->connect ()) {
        printf ("ok.\n");
    } else {
        printf ("failed.\n");
        exit (0);
    }

    if (connection->open ()) {
        std::string sentence;
        int count = 0;
        
        printf ("ok\n");

        while (true) {
            uint8_t byte;

            if (connection->read (sizeof (byte), & byte) > 0) {
                sentence.append (1, (char) byte);

                if (byte == 13) {
                    transmitter->send (& sentence.front (), sentence.length ());
                    printf ("Sentences forwarded %d\r", count ++);

                    sentence.clear ();
                }
            }
        }

        printf ("Closing connection.\n");

        connection->close ();
    } else {
        printf ("failed, error %d.\n", getErrorCode ());
    }

    delete connection;
    delete connCfg;

    exit (0);
}
