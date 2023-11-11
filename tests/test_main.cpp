#include "chat.hpp"
#include "fmt/core.h"

int main() {
    ChatServer server;
    while (true) {
        int c = server.accept();
        if (c == -1) {
            continue;
        }
        fmt::println("client fd: {}", c);
    }
}
