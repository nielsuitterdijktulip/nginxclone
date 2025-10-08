// Massive perfomance difference with step 02 (~50x with hey 10k 50 concurrent)
// Before, we did accept, read, write, close in one call before handling the next
// Now, we accept, read, write, close, asynchronously, only doing those that are ready
#include "client.h"
#include "client.c"

#include <stdio.h> // Console output
#include <stdlib.h> // for exit()
#include <string.h> // for memset(), memcpy(), strlen()
#include <unistd.h> // for close()
#include <arpa/inet.h> // for sockaddr_in, htons, inet_addr
#include <sys/epoll.h> // For epoll APIs
#include <fcntl.h> // filecontrol - to set non-blocking mode
#include <errno.h> // for error handling

#define PORT 8080
#define MAX_EVENTS 100

void make_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(1);
    }
}
int create_and_bind(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET for IPv4, SOCK_STREAM for TCP
    if (listen_fd < 0) { // If creation fails, exit
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = htons(port); // Port
    addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { 
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, SOMAXCONN) == -1) { // Backlog of 10 (max. queue)
        perror("listen"); 
        exit(1);
    }

    return listen_fd;
}
int accept_new_client(int listen_fd, int epoll_fd) {
    // accepts a new client connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("accept");
        }
        return -1;
    }
    
    struct client *c = client_create(client_fd);
    make_nonblocking(c->fd);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = c;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c.fd, &event) == -1) {
        perror("epoll_ctl: client_fd");
        client_free(c);
        return -1;
    }

    printf("Accepted new connection: fd %d\n", client_fd);
    return client_fd;
}
void handle_client(struct client *c) {
    // We read here all as we have EPOLLET set
    char buffer[1024];
    while (1) {
        // read(int fd, void buf[count], size_t count) -- attempts to read up to count bytes from fd into the buffer starting at buf.
        ssize_t count = read(c->fd, buffer, sizeof(buffer));
        if (count == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                client_free(c);
            }
            break;
        } else if (count == 0) {
            printf("Client fd %d disconnected\n", c->fd);
            client_free(c);
            break;
        }

        // Prepare HTTP response in write buffer
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 18\r\n"
            "\r\n"
            "Hello from server\n";

        size_t response_len = strlen(response);
        if (response_len <= WRITE_BUFFER_SIZE) {
            // Memory Copy - Copies response to write buffer
            memcpy(c->write_buffer, response, response_len);
            c->write_len = response_len;
            c->write_offset = 0;

            // Write response from buffer
            while (c->write_offset < c->write_len) {
                ssize_t w = write(c->fd, c->write_buffer + c->write_offset, c->write_len - c->write_offset);
                if (w == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("write");
                        client_free(c);
                    }
                    break;
                }
                c->write_offset += w;
            }
        }
    }
}

void run_event_loop(int listen_fd) {
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
        perror("epoll_ctl: listen_fd");
        exit(1);
    }

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == listen_fd) {
                // Accept all incoming connections
                while (accept_new_client(listen_fd, epfd) != -1) {}
            } else {
                handle_client((struct client*)events[i].data.ptr);
            }
        }
    }
}

int main() {
    int listen_fd = create_and_bind(PORT);
    make_nonblocking(listen_fd);
    printf("Listening on port %d...\n", PORT);

    run_event_loop(listen_fd);
    close(listen_fd);
    return 0;
}