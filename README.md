# WebServ - HTTP Server from Scratch


## About

This is a HTTP/1.1 compliant webserver built from scratch in C++. It is designed for efficiency, scalability, and configurability, using non-blocking I/O and a single epoll instance to handle clients simultaneously without blocking.

## Features

- **HTTP/1.1 compliant**: Supports GET, POST, and DELETE methods
- **Non-blocking I/O** through epoll
- **Nginx-like configuration**:
  - Multiple server blocks with customizable settings:
    - Port and host binding
    - Server name routing
    - Custom error pages
    - Client body size limits
  - Advanced routing and location blocks:
    - HTTP redirection
    - Directory listing (enable/disable)
    - Default index file selection
- **CGI support**:
  - Execute scripts in various languages (e.g., Python, Bash)
  - Configurable CGI timeout to prevent resource exhaustion
  - Restricts CGI execution to specific directories for security
- **Stress-tested** using Siege benchmarking tool

## Usage
```bash
# Clone the repository
git clone https://github.com/michmos/42_webserv.git

# Navigate to the project directory
cd 42_webserv

# Compile the project
make

# Create a new configuration file under configs/ or adjust an exisiting one
# (see example configs in configs/)

# Run the server with default configuration (running on localhost:8080)
./webserv

# Or specify a configuration file
./webserv config_file.conf
```


