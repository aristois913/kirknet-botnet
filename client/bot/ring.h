#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define RING_CAPACITY (1u << 16)
#define RING_MASK (RING_CAPACITY - 1u)

typedef struct {
    char buf[RING_CAPACITY];
    size_t head;
    size_t tail;
} ring_t;

static inline size_t ring_readable(const ring_t *r) {
    return r->head - r->tail;
}

static inline size_t ring_writable(const ring_t *r) {
    return RING_CAPACITY - ring_readable(r);
}

static inline bool ring_empty(const ring_t *r) {
    return r->head == r->tail;
}

static inline bool ring_full(const ring_t *r) {
    return ring_writable(r) == 0;
}

static inline size_t ring_write(ring_t *r, const char *src, size_t len) {
    size_t avail = ring_writable(r);
    if (len > avail)
        len = avail;

    if (!len)
        return 0;

    size_t off = r->head & RING_MASK;
    size_t first = RING_CAPACITY - off;

    if (len <= first) {
        memcpy(r->buf + off, src, len);
    } else {
        memcpy(r->buf + off, src, first);
        memcpy(r->buf, src + first, len - first);
    }

    r->head += len;
    return len;
}

static inline size_t ring_read(ring_t *r, char *dst, size_t len) {
    size_t avail = ring_readable(r);
    if (len > avail)
        len = avail;

    if (!len)
        return 0;

    size_t off = r->tail & RING_MASK;
    size_t first = RING_CAPACITY - off;

    if (len <= first) {
        memcpy(dst, r->buf + off, len);
    } else {
        memcpy(dst, r->buf + off, first);
        memcpy(dst + first, r->buf, len - first);
    }
    r->tail += len;
    return len;
}

static inline ssize_t ring_read_from_fd(ring_t *r, int fd) {
    size_t avail = ring_writable(r);
    if (!avail)
        return 0;

    size_t off = r->head & RING_MASK;
    size_t first = RING_CAPACITY - off;

    if (avail < first)
        first = avail;

    ssize_t n = read(fd, r->buf + off, first);
    if (n > 0)
        r->head += (size_t)n;

    return n;
}

static inline ssize_t ring_write_to_fd(ring_t *r, int fd) {
    size_t avail = ring_readable(r);
    if (!avail)
        return 0;

    size_t off = r->tail & RING_MASK;
    size_t first = RING_CAPACITY - off;

    if (avail < first)
        first = avail;

    ssize_t n = write(fd, r->buf + off, first);
    if (n > 0)
        r->tail += (size_t)n;

    return n;
}