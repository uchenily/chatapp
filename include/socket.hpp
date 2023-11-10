#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

int set_nonblock(int fd);
