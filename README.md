# Webserv


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
