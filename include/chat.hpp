#pragma once

#include "fmt/core.h"
#include "server.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#define SERVER_PORT 9000

struct Client {
    int fd;           // client socket fd
    std::string name; // client name
};

class ChatServer {
    static int max_clients;

public:
    ChatServer() : listen_fd{create_tcp_server(SERVER_PORT)} {
        assert(listen_fd != -1);
        // 支持的最大fd是1024
        clients.resize(1024);
    }

public:
    int accept() const;
    void create_client(int fd);
    void remove_client(int fd);
    void send_all(std::string message, int excluded);

public:
    // 保存最大的fd
    int max_clientfd = -1;

    int listen_fd = -1;
    int num_clients = 0;
    // 索引就是fd
    std::vector<std::unique_ptr<Client>> clients;
};
