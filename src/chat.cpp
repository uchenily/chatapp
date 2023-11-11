#include "chat.hpp"

#include "fmt/core.h"
#include "socket.hpp"

#include <memory>

int ChatServer::max_clients = 1024;

int ChatServer::accept() const { return accept_client(listen_fd); }

void ChatServer::create_client(int fd) {
    auto client = std::make_unique<Client>();
    set_nonblock(fd);
    client->fd = fd;
    client->name = fmt::format("user:{}", fd);

    clients[fd] = std::move(client);
    num_clients++;

    if (fd > max_clientfd) {
        max_clientfd = fd;
    }
}

void ChatServer::remove_client(int fd) {
    clients[fd] = nullptr;
    num_clients--;

    // 从高到底找新的max_clientfd
    for (int i = max_clients; i > 0; i--) {
        if (clients[i] != nullptr) {
            max_clientfd = i;
        }
    }
}

void ChatServer::send_all(std::string message, int excluded) {
    for (auto &client : clients) {
        if (client == nullptr || client->fd == excluded) {
            continue;
        }

        write(client->fd, message.data(), message.size());
    }
}
