# Step 1

Add client state & use adjust accept_client & handle_client

# Step 2

Detect a full HTTP request

- Read until you see `\r\n\r\n`
- Get method / path from first line

# Step 3

Prepare response /w keep-alive

# Step 4

Handle non-blocking writes

# Step 5

Handle keep-alive
