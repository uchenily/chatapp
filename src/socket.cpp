#include "socket.hpp"

int set_nonblock(int fd) {
    int yes = 1;

    int flags = fcntl(fd, F_GETFL);
    if ((flags) == -1) {
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }

    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    return 0;
}
