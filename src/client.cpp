#include "client.hpp"

#include "socket.hpp"

int tcp_connect(const std::string &addr, int port, bool nonblock) {
    int s = -1;
    int retval = -1;
    struct addrinfo hints, *servinfo{}, *p{};

    char portstr[6]; /* Max 16 bit number string length. */
    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(addr.data(), portstr, &hints, &servinfo) != 0) {
        return -1;
    }

    for (p = servinfo; p != nullptr; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == -1) {
            continue;
        }

        /* Put in non blocking state if needed. */
        if (nonblock && set_nonblock(s) == -1) {
            close(s);
            break;
        }

        /* Try to connect. */
        if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
            /* If the socket is non-blocking, it is ok for connect() to
             * return an EINPROGRESS error here. */
            if (errno == EINPROGRESS && nonblock) {
                freeaddrinfo(servinfo);
                return s;
            }

            /* Otherwise it's an error. */
            close(s);
            break;
        }

        /* If we ended an iteration of the for loop without errors, we
         * have a connected socket. Let's return to the caller. */
        retval = s;
        break;
    }

    /* After checking all the socket families we were not able
     * to connect. Return an error. */
    freeaddrinfo(servinfo);
    return retval; /* Will be -1 if no connection succeded. */
}
