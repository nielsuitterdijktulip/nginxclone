#include "client.h"
#include <stdlib.h> // for malloc, free
#include <string.h> // for memset

struct client *client_create(int fd) {
    struct client *c = malloc(sizeof(struct client));
    if (!c) return NULL;
    c->fd = fd;
    c->read_len = 0;
    c->write_len = 0;
    c->write_offset = 0;
    c->keep_alive = 0;
    memset(c->read_buffer, 0, READ_BUFFER_SIZE);
    memset(c->write_buffer, 0, WRITE_BUFFER_SIZE);
    return c;
}

void client_reset(struct client *c) {
    c->read_len = 0;
    c->write_len = 0;
    c->write_offset = 0;
    memset(c->read_buffer, 0, READ_BUFFER_SIZE);
    memset(c->write_buffer, 0, WRITE_BUFFER_SIZE);
    c->keep_alive = 0;
}

void client_free(struct client *c) {
    free(c);
}