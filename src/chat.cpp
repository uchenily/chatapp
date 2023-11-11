#include "chat.hpp"

#include "socket.hpp"

#include <memory>

void ChatServer::create_client(int fd) {
    auto client = std::make_unique<Client>();
    set_nonblock(fd);
    client->fd = fd;
    client->name = "";

    clients[fd] = std::move(client);
    num_clients++;
}

void ChatServer::remove_client(int fd) {
    clients[fd] = nullptr;
    num_clients--;
}
