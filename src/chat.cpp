#include "chat.hpp"

#include "fmt/core.h"
#include "socket.hpp"

#include <memory>

int ChatServer::accept() const { return accept_client(listen_sock); }

void ChatServer::create_client(int fd) {
    auto client = std::make_unique<Client>();
    set_nonblock(fd);
    client->fd = fd;
    client->name = fmt::format("user:{}", fd);

    clients[fd] = std::move(client);
    num_clients++;
}

void ChatServer::remove_client(int fd) {
    clients[fd] = nullptr;
    num_clients--;
}

void ChatServer::send_all(std::string message, int excluded) {
    for (auto &client : clients) {
        if (client == nullptr || client->fd == excluded) {
            continue;
        }

        write(client->fd, message.data(), message.size());
    }
}
