#include "connection.h"

int init_channel(const char *ip, int port, const char *name) {
    (void)name;

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }

    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    int bufsz = 1 << 20;

    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsz, sizeof(bufsz));
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsz, sizeof(bufsz));

    fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0));

    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        if (errno != EINPROGRESS) {
            perror("connect");
            exit(1);
        }

        struct pollfd pfd = {
            .fd = fd, .events = POLLOUT
        };

        int r = poll(&pfd, 1, 5000);

        if (r <= 0) {
            fputs("connect: timed out\n", stderr);
            exit(1);
        }

        int err = 0; socklen_t elen = sizeof(err);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &elen);
        if (err) {
            fprintf(stderr, "connect: %s\n", strerror(err));
            exit(1);
        }
    }

    return fd;
}