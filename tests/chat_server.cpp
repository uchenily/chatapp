#include "chat.hpp"
#include "fmt/core.h"

#include <cstdio>

#include <sys/select.h>

// 实现的功能:
// 1. 接受新的连接
// 2. 检查是否有client给我们发消息
// 3. 发送广播消息
int main() {
    ChatServer chat;
    while (true) {
        fd_set readfds;
        struct timeval tv;
        int retval{};

        FD_ZERO(&readfds);
        FD_SET(chat.listen_fd, &readfds);

        for (int i = 0; i <= chat.max_clientfd; i++) {
            if (chat.clients[i]) {
                FD_SET(i, &readfds);
            }
        }

        // 1秒超时
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // 找到最大的文件描述符
        int maxfd = chat.max_clientfd;
        if (maxfd < chat.listen_fd) {
            maxfd = chat.listen_fd;
        }
        retval = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);
        if (retval == -1) {
            fmt::println("select() error");
            exit(1);
        }

        if (static_cast<bool>(retval)) {
            // 如果是listen_fd, 则说明是一个新的客户端连接
            if (FD_ISSET(chat.listen_fd, &readfds)) {
                int fd = chat.accept();

                chat.create_client(fd);
                write(fd, "welcome!\n", 9);
                fmt::println("Connected client fd={}", fd);
            }

            char readbuf[1024];
            for (int i = 0; i <= chat.max_clientfd; i++) {
                if (chat.clients[i] == nullptr) {
                    continue;
                }
                // i这个文件描述符有数据(即这个client发送了消息)
                if (FD_ISSET(i, &readfds)) {
                    ssize_t nread = read(i, readbuf, sizeof(readbuf) - 1);
                    // 消息长度<=0, 说明这个client断开了
                    // -1: Error, 0: short read => socket已经close了
                    if (nread <= 0) {
                        fmt::println("Disconnected client fd={} name={}",
                                     i,
                                     chat.clients[i]->name);
                        chat.remove_client(i);
                    } else {
                        // 普通chat消息
                        readbuf[nread] = 0;

                        // 如果readbuf是一个命令, 则执行命令:
                        if (readbuf[0] == '/') {
                            // 移除空白字符
                            char *p = nullptr;
                            p = strchr(readbuf, '\r');
                            if (p) {
                                *p = 0;
                            }
                            p = strchr(readbuf, '\n');
                            if (p) {
                                *p = 0;
                            }

                            // 检查命令的参数
                            char *arg = strchr(readbuf, ' ');
                            if (arg) {
                                *arg = 0; /* Terminate command name. */
                                arg++; /* Argument is 1 byte after the space. */
                            }

                            if ((strcmp(readbuf, "/nick") == 0) && arg) {
                                chat.clients[i]->name = arg;
                            } else {
                                // Unsupported command. Send an error.
                                write(chat.clients[i]->fd,
                                      "Unsupported command\n",
                                      20);
                            }
                            continue;
                        }

                        auto boardcast_msg = fmt::format("{}> {}",
                                                         chat.clients[i]->name,
                                                         std::string(readbuf));
                        fmt::print("{}", boardcast_msg);

                        // 给除发送方以外的所有其他client发送消息
                        chat.send_all(boardcast_msg, i);
                    }
                }
            }
        } else {
            // Timeout
        }
    }
}
