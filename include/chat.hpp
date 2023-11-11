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

    ~Client() { fmt::println("~Client()"); }
};

class ChatServer {
public:
    ChatServer() : listen_sock{create_tcp_server(SERVER_PORT)} {
        assert(listen_sock != -1);
        // 支持的最大fd是1024
        clients.resize(1024);
    }

public:
    int accept() const;
    void create_client(int fd);
    void remove_client(int fd);
    void send_all(std::string message, int excluded);

private:
    int listen_sock = -1;
    int num_clients = 0;
    // 索引就是fd
    std::vector<std::unique_ptr<Client>> clients;
};
