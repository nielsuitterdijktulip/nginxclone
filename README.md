# Nginx Clone

The goal of this project is to create a simplified version of the Nginx web server from scratch. This project is divided into multiple phases, each focusing on different aspects of web server functionality.

# Phase 1 - TCP Server & Event Loop

- Implement TCP listener in C
- Make non-blocking I/O using epoll
- Handle multiple connections concurrently and echo back data

# Phase 2 - HTTP Parser & Response

- Parse basic HTTP/1.1 requests manually
- Respond with static text & simple HTML
- Handle persistent connections

# Phase 3 - Static File Server & Zero-Copy

- Serve files from disk
- use sendfile() syscall for zero-copy
- Implement basic MIME type handling

# Phase 4 - Reverse Proxy & Load Balancing

- Forward requests to backend servers
- Implement round-robin or least-connections load balancing
- Stream responses without buffering everything in memory

# Phase 5 - Worker Model & Config Reloading

- Fork multiple worker processes
- Use shared memory for statistics / counters / caching
- Implement config reload without dropping active connections

# Phase 6 - TLS & Benchmarking

- Terminate HTTPS connections using OpenSSL
- Benchmark against Go version or nginx for curiousity

---

# Step 1

Get C working

# Step 2

Get a TCP server working

# Step 3

Setting up epoll for non-blocking I/O

# Step 4
