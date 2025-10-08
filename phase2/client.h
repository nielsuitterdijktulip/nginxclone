#ifndef CLIENT_H
#define CLIENT_H

#include <stddef.h> // for size_t

#define READ_BUFFER_SIZE 4096
#define WRITE_BUFFER_SIZE 4096


struct client {
    int fd;
    char read_buffer[READ_BUFFER_SIZE];
    size_t read_len;
    char write_buffer[WRITE_BUFFER_SIZE];
    size_t write_len;
    size_t write_offset;
    int keep_alive;
};

struct client *client_create(int fd);
void client_reset(struct client *c);
void client_free(struct client *c);

#endif // CLIENT_H