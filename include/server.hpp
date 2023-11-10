#pragma once

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

int create_tcp_server(int port);

int accept_client(int server_socket);
