#include <stdio.h> // Console output
#include <stdlib.h> // for exit()
#include <string.h> // for memset()
#include <unistd.h> // for close()
#include <arpa/inet.h> // for sockaddr_in, htons, inet_addr

int main() {
    int listen_fd;
    struct sockaddr_in addr; // Assign addr with type sockaddr_in (IPv4)

    // Create TCP socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET for IPv4, SOCK_STREAM for TCP
    if (listen_fd < 0) { // If creation fails, exit
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = htons(8080); // Port 8080
    addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address

    // Binds socket, map addr to generic sockaddr
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { 
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(listen_fd, 10) < 0) { // Backlog of 10 (max. queue)
        perror("listen"); 
        exit(1);
    }

    printf("Listening on port 8080...\n");

    while (1) {
        int client_fd = accept(listen_fd, NULL, NULL);
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 18\r\n"
            "\r\n"
            "Hello from server\n";
        if (client_fd >= 0) {
            printf("Accepted a connection!\n");
            write(client_fd, response, strlen(response));
            close(client_fd);
        }
    }

    return 0;
}