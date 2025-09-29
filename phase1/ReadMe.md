# Step 1

Get C working -- `sudo apt update && sudo apt install build-essential`
Make a simple "Hello, World!" program and compile it using `gcc hello.c -o hello` and run it using `./hello`

# Step 2

Build a simple TCP server using sockets. Use `socket()`, `bind()`, `listen()`, and `accept()` system calls to create a server that listens on a specified port and accepts incoming connections.

# Step 3

Add epoll for non-blocking I/O. Use `epoll_create1()`, `epoll_ctl()`, and `epoll_wait()` to monitor multiple file descriptors to see if I/O is possible on any of them.
