#include "io_loop.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>
#include <errno.h>

#define FD_STDIN   0
#define FD_SOCK    1
#define FD_STDOUT  2
#define FD_COUNT   3

void io_loop(int sock_fd) {
    ring_t tx = {0};
    ring_t rx = {0};

    struct pollfd fds[FD_COUNT] = {
        [FD_STDIN] = {
            .fd = STDIN_FILENO, .events = POLLIN
        },
        [FD_SOCK] = {
            .fd = sock_fd, .events = POLLIN
        },
        [FD_STDOUT] = {
            .fd = STDOUT_FILENO, .events = 0
        },
    };

    for (;;) {

        fds[FD_SOCK].events = POLLIN | (!ring_empty(&tx) ? POLLOUT : 0);
        fds[FD_STDOUT].events = !ring_empty(&rx) ? POLLOUT : 0;
        fds[FD_STDIN].events = !ring_full(&tx) ? POLLIN  : 0;

        int n = poll(fds, FD_COUNT, -1);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            return;
        }

        if (fds[FD_STDIN].revents & POLLIN) {
            ssize_t r = ring_read_from_fd(&tx, STDIN_FILENO);

            if (r == 0) {
                fds[FD_STDIN].events = 0;
            } else if (r < 0 && errno != EAGAIN && errno != EINTR) {
                perror("read(stdin)");
                return;
            }
        }

        if (fds[FD_SOCK].revents & POLLOUT) {
            ssize_t w = ring_write_to_fd(&tx, sock_fd);
            if (w < 0 && errno != EAGAIN && errno != EINTR) {
                perror("write(socket)");
                return;
            }
        }

        if (fds[FD_SOCK].revents & POLLIN) {
            ssize_t r = ring_read_from_fd(&rx, sock_fd);

            if (r == 0) {
                while (!ring_empty(&rx))
                    ring_write_to_fd(&rx, STDOUT_FILENO);
                fputs("\n[disconnected]\n", stderr);
                return;
            }

            if (r < 0 && errno != EAGAIN && errno != EINTR) {
                perror("read(socket)");
                return;
            }
        }

        if (fds[FD_STDOUT].revents & POLLOUT) {
            ssize_t w = ring_write_to_fd(&rx, STDOUT_FILENO);

            if (w < 0 && errno != EAGAIN && errno != EINTR) {
                perror("write(stdout)");
                return;
            }
        }

        if (fds[FD_SOCK].revents & (POLLERR | POLLHUP)) {
            fputs("[socket error/hangup]\n", stderr);
            return;
        }
    }
}