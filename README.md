# Webserv


# HTTP
- Understand HTTP request/response structure.
- Implement GET, POST, DELETE methods.
- Correctly parse headers and bodies.
- Serve static files & custom error pages.
- Handle persistent connections (keep-alive).
- Return proper status codes.

## Understanding HTTP Headers

### **1. Structure of HTTP Headers**

Each header follows a simple **key-value pair** format:

```
Header-Name: Header-Value
```

Headers are sent as part of the HTTP request or response, separated from the body by a blank line.

#### **1.1 HTTP Request Headers Structure**

A typical HTTP request consists of:

1. **Request Line** (method, URL, HTTP version)
2. **Headers** (metadata about the request)
3. **Empty Line** (separator)
4. **Body** (optional, mainly for POST/PUT requests)

Example:

```
GET /page.html HTTP/1.1
Host: www.example.com
User-Agent: curl/7.68.0
Accept: text/html,application/xhtml+xml
```

- The **request line** specifies the HTTP method (`GET`), resource (`/page.html`), and version (`HTTP/1.1`).
- The **headers** provide additional context.
- A **blank line** separates headers from the body.

#### **1.2 HTTP Response Headers Structure**

A response contains:

1. **Status Line** (protocol, status code, message)
2. **Headers** (metadata about the response)
3. **Empty Line**
4. **Body** (the actual content)

Example:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 125
Connection: keep-alive

<html>...</html>
```

- The **status line** includes `HTTP/1.1 200 OK`.
- The **headers** define response properties.
- A **blank line** separates the headers from the content.

### **2. Common HTTP Headers and Their Usage**

#### **2.1 Request Headers**

| Header        | Description                              | Example                                         |
| ------------- | ---------------------------------------- | ----------------------------------------------- |
| Host          | Specifies the target host                | `Host: www.example.com`                         |
| User-Agent    | Identifies the client making the request | `User-Agent: Mozilla/5.0`                       |
| Accept        | Specifies acceptable response formats    | `Accept: text/html, application/json`           |
| Authorization | Provides authentication credentials      | `Authorization: Basic dXNlcm5hbWU6cGFzc3dvcmQ=` |
| Cache-Control | Controls caching behavior                | `Cache-Control: no-cache`                       |

#### **2.2 Response Headers**

| Header         | Description                             | Example                                  |
| -------------- | --------------------------------------- | ---------------------------------------- |
| Content-Type   | Specifies the MIME type of the response | `Content-Type: text/html`                |
| Content-Length | Indicates the size of the response body | `Content-Length: 5120`                   |
| Server         | Identifies the web server software      | `Server: Apache/2.4.41`                  |
| Set-Cookie     | Sets a cookie in the client             | `Set-Cookie: sessionId=abc123; HttpOnly` |
| Location       | Redirects the client to another URL     | `Location: https://www.new-url.com`      |

### **3. How to Use and Understand HTTP Headers**

#### **3.1 Debugging with Headers**

You can inspect headers using:

- **Browser Developer Tools** (Network tab in Chrome/Firefox)
- **Curl command-line tool**:
  ```sh
  curl -I https://example.com
  ```
- **Telnet/Netcat** for manual inspection:
  ```sh
  telnet example.com 80
  GET / HTTP/1.1
  Host: example.com
  ```

#### **3.2 Using Headers for Security**

- **Strict-Transport-Security (HSTS):** Enforces HTTPS connections.
  ```
  Strict-Transport-Security: max-age=31536000; includeSubDomains
  ```
- **X-Frame-Options:** Prevents clickjacking attacks.
  ```
  X-Frame-Options: DENY
  ```
- **Content-Security-Policy (CSP):** Controls allowed sources of content.
  ```
  Content-Security-Policy: default-src 'self'
  ```


# epoll (Non-Blocking I/O)
epoll is a scalable I/O event notification mechanism in Linux, designed to efficiently handle multiple file descriptors. It is commonly used in high-performance network applications, such as servers, where handling thousands of concurrent connections is required.

### Key Features of epoll:
1. **Efficient Event Notification**: Unlike `select()` and `poll()`, `epoll` avoids scanning file descriptors linearly, making it more scalable.
2. **Edge-Triggered (ET) vs. Level-Triggered (LT) Modes:**  
   - **Level-Triggered (default)**: Events trigger as long as the condition persists.  
   - **Edge-Triggered (`EPOLLET`)**: Events trigger only when the state changes (e.g., new data arrives).
3. **Handles Many Connections**: `epoll` is designed to efficiently handle thousands of connections.

### **Using ****************`epoll()`**************** in Webserv**

In Webserv, the project requires using **only one ****************`epoll()`**************** instance** to handle all I/O operations. This means that every socket operation—whether it is accepting new connections, reading from a client, or writing responses—must be coordinated using `epoll`.

#### **How ****************`epoll()`**************** Works**

1. **Create an ****************`epoll`**************** instance** using `epoll_create1()`.

     **`epoll_create1(int flags)`** (or deprecated `epoll_create(int size)`)  
     - Creates an epoll instance and returns a file descriptor referring to it.  
     - **flags**: Can be `EPOLL_CLOEXEC` to set the close-on-exec flag.
2. **Register file descriptors** (sockets) with `epoll_ctl()`.

   **`epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`**  
   - Controls the epoll instance by adding, modifying, or removing file descriptors.  
   - **op**: Can be `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, or `EPOLL_CTL_DEL`.
3. **Wait for events** using `epoll_wait()`.

   **`epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`**  
   - Waits for events on the epoll instance.  
   - **maxevents**: Maximum number of events to return.  
   - **timeout**: Milliseconds to wait (`0` = non-blocking, `-1` = wait indefinitely).
4. **Process the event** based on which file descriptors are ready.
5. **Repeat the process** to handle subsequent I/O operations.

#### **Example Workflow**

1. **Server starts and listens on a socket**
   - Calls `socket()` and `bind()` to set up a listening socket.
   - Calls `listen()` to start accepting connections.
   - Creates an `epoll` instance using `epoll_create1()`.
   - Adds the listening socket to the `epoll` instance using `epoll_ctl()` with `EPOLLIN`.
2. **Handling new connections**
   - When `epoll_wait()` indicates a new connection, `accept()` is called.
   - The new client socket is added to `epoll` with `EPOLLIN` to monitor read events.
3. **Handling client requests**
   - If a client socket has data ready to read (`EPOLLIN`), `recv()` is used to read it.
   - The server processes the request and prepares a response.
   - The client socket is modified in `epoll_ctl()` to monitor for writing (`EPOLLOUT`).
4. **Sending responses**
   - When `epoll_wait()` detects that the socket is ready for writing, `send()` is used to transmit data.
   - Once done, the socket may be closed or kept open for persistent connections.

### **Important Constraints for Webserv**

- **Only one ****************`epoll`**************** instance** must be used to manage all I/O operations.
- **Reading and writing must both be monitored** within the same `epoll` instance.
- **No direct ****************`read()`**************** or ****************`write()`**************** calls** without first using `epoll_wait()` to check readiness.
- **Using ****************`errno`**************** after a ****************`read()`**************** or ****************`write()`**************** call is forbidden.**
- **Non-blocking file descriptors** must be used to avoid waiting indefinitely on slow or unresponsive clients.



#  Configuration File for Webserv

The configuration file is a crucial component of Webserv as it allows defining how the server behaves, including which ports it listens to, which files it serves, and how it handles requests. Inspired by NGINX’s configuration, this file provides flexibility and structure to the server’s operation.

### **Key Features of the Configuration File**

1. **Defining Server Blocks**

   - Each server is identified by a host and a port number.
   - Multiple servers can be defined within a single configuration file.

2. **Setting Up Server Names**

   - Allows defining `server_name` for different virtual hosts.
   - The first defined server on a host\:port combination acts as the default.

3. **Error Page Configuration**

   - Custom error pages can be set for specific HTTP error codes.

4. **Client Request Limitations**

   - Restricting the maximum body size for client requests to prevent overload.

5. **Defining Routes**

   - Each route specifies how a request is handled.
   - Supported route options:
     - **Allowed HTTP Methods** (`GET`, `POST`, `DELETE`)
     - **Redirections** (301, 302 status codes)
     - **Static File Directory Mappings**
     - **Directory Listing Settings**
     - **CGI Execution Settings**
     - **File Upload Configuration**

### **Example Configuration File**

#### Basic Example:

```
server {
    listen 8080;
    server_name example.com;

    error_page 404 /errors/404.html;
    client_max_body_size 10M;

    location / {
        root /var/www/html;
        index index.html;
    }

    location /uploads/ {
        root /var/www/uploads;
        methods POST GET;
    }

    location /cgi-bin/ {
        cgi_enable on;
        cgi_extension .php;
        root /var/www/cgi-bin;
    }
}
```

### **Breakdown of Example Configuration**

- **Server Block**
  - The server listens on port `8080` and responds to `example.com`.
- **Error Page Setup**
  - Defines a custom `404` error page stored at `/errors/404.html`.
- **Client Limitations**
  - Restricts the body size of incoming requests to `10MB`.
- **Root Location (********`/`********)**
  - Serves static files from `/var/www/html`, with `index.html` as the default page.
- **Uploads Location (********`/uploads/`********)**
  - Enables `POST` and `GET` methods, storing files in `/var/www/uploads/`.
- **CGI Configuration (********`/cgi-bin/`********)**
  - Enables CGI execution for `.php` files in `/var/www/cgi-bin/`.

### **Advanced Features**

1. **Multiple Server Blocks**
   - Supports different domains on different ports.
2. **IP-based Virtual Hosting**
   - Specific IPs can be assigned per server block.
3. **Multiple Methods per Route**
   - Routes can allow `GET`, `POST`, and `DELETE` selectively.
4. **Nested Routing**
   - Subroutes can inherit properties from parent routes.




# Links:

#### epoll
https://man7.org/linux/man-pages/man7/epoll.7.html


#### HTTP codes
https://www.w3schools.com/tags/ref_httpmessages.asp

https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
