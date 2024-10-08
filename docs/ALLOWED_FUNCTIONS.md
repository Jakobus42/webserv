# ALLOWED_FUNCTIONS.md

## Introduction
This document lists all the allowed functions for your project and provides a brief overview of their usage along with links to their official documentation for further reading. The goal is to help understand the basics of each function's behavior and how they can be used effectively.

---

### 1. `execve`
- **Usage**: Executes a program, replacing the current process image with a new one.
- **Link**: [execve - Linux man page](https://man7.org/linux/man-pages/man2/execve.2.html)

### 2. `dup` & `dup2`
- **Usage**: `dup` duplicates an existing file descriptor. `dup2` duplicates it to a specified file descriptor.
- **Link**: [dup, dup2 - Linux man page](https://man7.org/linux/man-pages/man2/dup.2.html)

### 3. `pipe`
- **Usage**: Creates a unidirectional data channel (a pipe) for inter-process communication.
- **Link**: [pipe - Linux man page](https://man7.org/linux/man-pages/man2/pipe.2.html)

### 4. `strerror` & `gai_strerror`
- **Usage**: `strerror` returns a string describing an error code. `gai_strerror` returns an error description for `getaddrinfo` errors.
- **Link**: [strerror - Linux man page](https://man7.org/linux/man-pages/man3/strerror.3.html)

### 5. `errno`
- **Usage**: Global variable indicating the error code from the last system call.
- **Link**: [errno - Linux man page](https://man7.org/linux/man-pages/man3/errno.3.html)

### 6. `fork`
- **Usage**: Creates a new process by duplicating the calling process.
- **Link**: [fork - Linux man page](https://man7.org/linux/man-pages/man2/fork.2.html)

### 7. `socketpair`
- **Usage**: Creates a pair of connected sockets for inter-process communication.
- **Link**: [socketpair - Linux man page](https://man7.org/linux/man-pages/man2/socketpair.2.html)

### 8. `htons`, `htonl`, `ntohs`, `ntohl`
- **Usage**: Functions to convert values between host and network byte order.
- **Link**: [byteorder - Linux man page](https://man7.org/linux/man-pages/man3/htons.3.html)

### 9. `select`
- **Usage**: Monitors multiple file descriptors to see if I/O is possible.
- **Link**: [select - Linux man page](https://man7.org/linux/man-pages/man2/select.2.html)

### 10. `poll`
- **Usage**: Monitors multiple file descriptors for I/O readiness.
- **Link**: [poll - Linux man page](https://man7.org/linux/man-pages/man2/poll.2.html)

### 11. `epoll` (`epoll_create`, `epoll_ctl`, `epoll_wait`)
- **Usage**: A more scalable I/O event notification facility compared to `select` or `poll`.
- **Link**: [epoll - Linux man page](https://man7.org/linux/man-pages/man7/epoll.7.html)

### 12. `kqueue` (`kqueue`, `kevent`)
- **Usage**: Provides a method for scalable I/O event notification (primarily BSD systems).
- **Link**: [kqueue - FreeBSD man page](https://man.freebsd.org/cgi/man.cgi?query=kqueue&sektion=2)

### 13. `socket`, `accept`, `listen`, `send`, `recv`
- **Usage**: Network communication functions. `socket` creates a socket, `accept` handles incoming connections, `listen` listens for connections, `send` and `recv` handle data transfer.
- **Link**: [socket - Linux man page](https://man7.org/linux/man-pages/man2/socket.2.html)

### 14. `chdir`
- **Usage**: Changes the current working directory.
- **Link**: [chdir - Linux man page](https://man7.org/linux/man-pages/man2/chdir.2.html)

### 15. `bind`, `connect`
- **Usage**: `bind` associates a socket with an address, `connect` connects to a remote host.
- **Link**: [bind, connect - Linux man page](https://man7.org/linux/man-pages/man2/bind.2.html)

### 16. `getaddrinfo`, `freeaddrinfo`
- **Usage**: Resolves hostnames and service names into socket addresses. `freeaddrinfo` deallocates the result.
- **Link**: [getaddrinfo - Linux man page](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)

### 17. `setsockopt`, `getsockname`
- **Usage**: `setsockopt` sets options on a socket, `getsockname` gets the current address of a socket.
- **Link**: [setsockopt - Linux man page](https://man7.org/linux/man-pages/man2/setsockopt.2.html)

### 18. `getprotobyname`
- **Usage**: Retrieves protocol information by its name.
- **Link**: [getprotobyname - Linux man page](https://man7.org/linux/man-pages/man3/getprotoent.3.html)

### 19. `fcntl`
- **Usage**: Performs operations on file descriptors.
- **Link**: [fcntl - Linux man page](https://man7.org/linux/man-pages/man2/fcntl.2.html)

### 20. `close`, `read`, `write`
- **Usage**: File descriptor operations. `close` closes a file descriptor, `read` reads data, and `write` writes data.
- **Link**: [close, read, write - Linux man page](https://man7.org/linux/man-pages/man2/read.2.html)

### 21. `waitpid`, `kill`, `signal`
- **Usage**: Process control functions. `waitpid` waits for a process, `kill` sends signals, and `signal` handles signals.
- **Link**: [waitpid, kill, signal - Linux man page](https://man7.org/linux/man-pages/man2/waitpid.2.html)

### 22. `access`, `stat`
- **Usage**: `access` checks file permissions, `stat` retrieves file status.
- **Link**: [access, stat - Linux man page](https://man7.org/linux/man-pages/man2/stat.2.html)

### 23. `open`, `opendir`, `readdir`, `closedir`
- **Usage**: File and directory operations. `open` opens a file, `opendir` opens a directory, `readdir` reads directory contents, and `closedir` closes a directory.
- **Link**: [open, opendir, readdir, closedir - Linux man page](https://man7.org/linux/man-pages/man2/open.2.html)

---

## Notes
This document aims to provide a quick overview of the allowed functions and should be supplemented with thorough reading of the official documentation before usage in code. Make sure to handle error codes and edge cases appropriately for production-ready code.

For more in-depth details, please refer to the linked official manual pages.

