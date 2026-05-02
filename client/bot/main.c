#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "macros.h"
#include "connection.h"
#include "io_loop.h"

static void set_nonblock(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0));
}

int main(void) {
    char *name = getenv("USER");

    set_nonblock(STDIN_FILENO);
    set_nonblock(STDOUT_FILENO);

    int sock = init_channel(SERVER, PORT, name);

    io_loop(sock);

    close(sock);
    return 0;
}