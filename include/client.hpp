#pragma once

#include <netdb.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

int tcp_connect(const std::string &addr, int port, bool nonblock = false);
