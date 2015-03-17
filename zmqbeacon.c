/*
 * Copyright 2014 Florian Schäfer <florian.schaefer@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holders.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <czmq.h>

#include "config.h"

void
print_usage()
{
    fprintf(stderr, "%s %s - LAN service announcement and discovery\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stderr, "\n");
    fprintf(stderr, "  Usage for LAN service announcement:\n");
    fprintf(stderr, "  %s -a announcement [-i interval]\n", PACKAGE_NAME);
    fprintf(stderr, "    -i  --interval     : beaconing interval in ms (default: 1000)\n");
    fprintf(stderr, "    -a  --announcement : announcement to send;\n");
    fprintf(stderr, "                         Instead of -a, the announcement can be set\n");
    fprintf(stderr, "                         via environment variable ZMQBEACON_DATA.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Usage for LAN service discovery:\n");
    fprintf(stderr, "  %s -l [-t timeout] [-s filter] [-r repeat] [-c command]\n", PACKAGE_NAME);
    fprintf(stderr, "    -l  --listen       : listen for beacons to arrive\n");
    fprintf(stderr, "    -t  --timeout      : wait for at most ms (default: -1 = forever)\n");
    fprintf(stderr, "    -s  --subscribe    : subscribe to messages; no filter gets everything\n");
    fprintf(stderr, "    -r  --repeat       : repeat listening to beacons (default: 1)\n");
    fprintf(stderr, "    -c  --command      : execute shell command if a message is received;\n");
    fprintf(stderr, "                         Received data is stored in environment variables\n");
    fprintf(stderr, "                         ZMQBEACON_IP and ZMQBEACON_DATA.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Additional options for both modes:\n");
    fprintf(stderr, "  %s ... [-p port] [-v]\n", PACKAGE_NAME);
    fprintf(stderr, "    -p  --port         : UDP port (default: 5670)\n");
    fprintf(stderr, "    -v  --verbose      : verbose output to stderr\n");
    fprintf(stderr, "    -h  --help         : display this usage information\n");
    fprintf(stderr, "\n");
}

enum {
    ZMQBEACON_PUBLISH,
    ZMQBEACON_LISTEN
};

int
main(int argc, char *argv[])
{

    char *announcement = getenv("ZMQBEACON_DATA");
    int interval = 1000;

    int mode = ZMQBEACON_PUBLISH;
    int timeout = -1;

    char *filter = NULL;
    int repeat = 1;
    char *command = NULL;

    int port = 5670;
    int verbose = 0;

    const char* const short_options = "a:i:lt:s:r:c:p:v";
    const struct option long_options[] = {
        { "interval",     1, NULL, 'i' },
        { "announcement", 1, NULL, 'a' },
        { "listen",       1, NULL, 'l' },
        { "timeout",      1, NULL, 't' },
        { "subscribe",    1, NULL, 's' },
        { "repeat",       1, NULL, 'r' },
        { "command",      1, NULL, 'c' },
        { "port",         1, NULL, 'p' },
        { "verbose",      0, NULL, 'v' },
        { "help",         0, NULL, 'h' },
        { NULL,           0, NULL,  0  }
    };

    char c;
    while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (c) {
        case 'a':
            announcement = optarg;
            break;
        case 'i':
            interval = atoi(optarg);
            break;

        case 'l':
            mode = ZMQBEACON_LISTEN;
            break;
        case 't':
            timeout = atoi(optarg);
            break;
        case 's':
            filter = optarg;
            break;
        case 'r':
            repeat = atoi(optarg);
            if (repeat == 0) {
                repeat = 1;
            }
            break;
        case 'c':
            command = optarg;
            break;

        case 'p':
            port = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;

        default:
            print_usage();
            return 1;
        }
    }

    if (mode == ZMQBEACON_PUBLISH && announcement == NULL) {
        print_usage();
        return 1;
    }

#ifdef HAVE_GETEUID
    if (port < 1024 && geteuid() != 0) {
        fprintf(stderr, "error: %s\n", "user does not have permission to use privileged ports");
        return 1;
    }
#endif

    zbeacon_t* beacon = zbeacon_new(port);

    if (mode == ZMQBEACON_PUBLISH) {

        zbeacon_set_interval(beacon, interval);
        zbeacon_publish(beacon, (byte*)announcement, announcement == NULL ? 0 : strlen(announcement));

        while (zctx_interrupted == 0) {
            zclock_sleep(1000);
        }

    }
    else if (mode == ZMQBEACON_LISTEN) {

        zsocket_set_rcvtimeo(zbeacon_socket(beacon), timeout);

        char *ipaddress = NULL;
        char *data = NULL;

        do {
            zbeacon_subscribe(beacon, (byte*)filter, filter == NULL ? 0 : strlen(filter));

            ipaddress = zstr_recv(zbeacon_socket(beacon));

            if (!ipaddress) {
                break;
            }

            data = zstr_recv(zbeacon_socket(beacon));

            setenv("ZMQBEACON_IP", ipaddress, 1);
            setenv("ZMQBEACON_DATA", data, 1);

            if (verbose && ipaddress && data) {
                fprintf(stderr, "received data from %s:%s", ipaddress, data);
            }

            if (command) {
                int ret = system(command);
                if ((WIFSIGNALED(ret) && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)) || ret != 0) {
                    return ret;
                }
            }

            free(ipaddress);
            free(data);

        } while ((repeat < 0 || --repeat != 0) && zctx_interrupted == 0);

        unsetenv("ZMQBEACON_IP");
        unsetenv("ZMQBEACON_DATA");

    }

    zbeacon_destroy(&beacon);

    return 0;
}
