#include "server.hpp"

int create_tcp_server(int port) {
    struct sockaddr_in sa {};

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        return -1;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) == -1
        || listen(s, 511) == -1) {
        close(s);
        return -1;
    }

    return s;
}

int accept_client(int server_socket) {
    int s{};
    struct sockaddr_in sa;
    socklen_t len{sizeof(sa)};
    while (true) {
        s = accept(server_socket, (struct sockaddr *) &sa, &len);
        if (s == -1) {
            if (errno == EINTR) {
                continue; // Try again
            }
            return -1;
        }
        break;
    }
    return s;
}
