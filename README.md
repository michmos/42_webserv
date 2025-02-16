# Webserv


## HTTP steps
- Understand HTTP request/response structure.
- Implement GET, POST, DELETE methods.
- Correctly parse headers and bodies.
- Serve static files & custom error pages.
- Handle persistent connections (keep-alive).
- Return proper status codes.

**Understanding HTTP Headers**

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




## epoll in C++
epoll is a scalable I/O event notification mechanism in Linux, designed to efficiently handle multiple file descriptors. It is commonly used in high-performance network applications, such as servers, where handling thousands of concurrent connections is required.

### Key Functions:

1. **`epoll_create1(int flags)`** (or deprecated `epoll_create(int size)`)  
   - Creates an epoll instance and returns a file descriptor referring to it.  
   - **flags**: Can be `EPOLL_CLOEXEC` to set the close-on-exec flag.

2. **`epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`**  
   - Controls the epoll instance by adding, modifying, or removing file descriptors.  
   - **op**: Can be `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, or `EPOLL_CTL_DEL`.

3. **`epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`**  
   - Waits for events on the epoll instance.  
   - **maxevents**: Maximum number of events to return.  
   - **timeout**: Milliseconds to wait (`0` = non-blocking, `-1` = wait indefinitely).

### Key Features of epoll:

1. **Efficient Event Notification**: Unlike `select()` and `poll()`, `epoll` avoids scanning file descriptors linearly, making it more scalable.
2. **Edge-Triggered (ET) vs. Level-Triggered (LT) Modes:**  
   - **Level-Triggered (default)**: Events trigger as long as the condition persists.  
   - **Edge-Triggered (`EPOLLET`)**: Events trigger only when the state changes (e.g., new data arrives).
3. **Handles Many Connections**: `epoll` is designed to efficiently handle thousands of connections.






## Links:

#### epoll
https://man7.org/linux/man-pages/man7/epoll.7.html


#### HTTP codes
https://www.w3schools.com/tags/ref_httpmessages.asp

https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
