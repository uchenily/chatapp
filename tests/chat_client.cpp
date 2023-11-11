#include "client.hpp"
#include "fmt/core.h"
#include "terminal.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        fmt::println("Usage: {} <host> <port>", argv[0]);
        exit(1);
    }

    /* Create a TCP connection with the server. */
    int s = tcp_connect(argv[1], atoi(argv[2]));
    if (s == -1) {
        fmt::println("Failed to connect to server");
        exit(1);
    }

    /* Put the terminal in raw mode: this way we will receive every
     * single key stroke as soon as the user types it. No buffering
     * nor translation of escape sequences of any kind. */
    setRawMode(fileno(stdin), true);

    /* Wait for the standard input or the server socket to
     * have some data. */
    fd_set readfds;
    int stdin_fd = fileno(stdin);

    struct InputBuffer ib;
    inputBufferClear(&ib);

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);
        FD_SET(stdin_fd, &readfds);
        int maxfd = s > stdin_fd ? s : stdin_fd;

        int num_events = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (num_events == -1) {
            fmt::println("select() error");
            exit(1);
        }

        if ((bool) num_events) {
            char buf[128]; /* Generic buffer for both code paths. */

            if (FD_ISSET(s, &readfds)) {
                /* Data from the server */
                ssize_t count = read(s, buf, sizeof(buf));
                if (count <= 0) {
                    fmt::println("Connection lost");
                    exit(1);
                }
                inputBufferHide(&ib);
                write(fileno(stdout), buf, count);
                inputBufferShow(&ib);
            } else if (FD_ISSET(stdin_fd, &readfds)) {
                /* Data from the user typing on the terminal */
                ssize_t count = read(stdin_fd, buf, sizeof(buf));
                for (int j = 0; j < count; j++) {
                    int res = inputBufferFeedChar(&ib, buf[j]);
                    switch (res) {
                        case IB_GOTLINE:
                            inputBufferAppend(&ib, '\n');
                            inputBufferHide(&ib);
                            write(fileno(stdout), "you> ", 5);
                            write(fileno(stdout), ib.buf, ib.len);
                            write(s, ib.buf, ib.len);
                            inputBufferClear(&ib);
                            break;
                        case IB_OK:
                            break;
                    }
                }
            }
        }
    }
    close(s);
}
