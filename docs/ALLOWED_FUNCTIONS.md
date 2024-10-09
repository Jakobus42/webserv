# ALLOWED_FUNCTIONS.md

1. **execve**
   - **Usage**: Executes a program, replacing the current process.
   - **Link**: [execve documentation](https://man7.org/linux/man-pages/man2/execve.2.html)

2. **dup**
   - **Usage**: Duplicates an existing file descriptor.
   - **Link**: [dup documentation](https://man7.org/linux/man-pages/man2/dup.2.html)

3. **dup2**
   - **Usage**: Duplicates a file descriptor to a given descriptor number.
   - **Link**: [dup2 documentation](https://man7.org/linux/man-pages/man2/dup.2.html)

4. **pipe**
   - **Usage**: Creates a unidirectional data channel that can be used for inter-process communication.
   - **Link**: [pipe documentation](https://man7.org/linux/man-pages/man2/pipe.2.html)

5. **strerror**
   - **Usage**: Returns a string that describes the error code passed.
   - **Link**: [strerror documentation](https://man7.org/linux/man-pages/man3/strerror.3.html)

6. **gai_strerror**
   - **Usage**: Returns a string describing the error code returned by `getaddrinfo()`.
   - **Link**: [gai_strerror documentation](https://man7.org/linux/man-pages/man3/gai_strerror.3.html)

7. **errno**
   - **Usage**: Global variable that indicates the error number for the last system call.
   - **Link**: [errno documentation](https://man7.org/linux/man-pages/man3/errno.3.html)

8. **fork**
   - **Usage**: Creates a new process by duplicating the current process.
   - **Link**: [fork documentation](https://man7.org/linux/man-pages/man2/fork.2.html)

9. **socketpair**
   - **Usage**: Creates a pair of connected sockets.
   - **Link**: [socketpair documentation](https://man7.org/linux/man-pages/man2/socketpair.2.html)

10. **htons**
    - **Usage**: Converts a short integer from host to network byte order.
    - **Link**: [htons documentation](https://man7.org/linux/man-pages/man3/htons.3p.html)

11. **htonl**
    - **Usage**: Converts a long integer from host to network byte order.
    - **Link**: [htonl documentation](https://man7.org/linux/man-pages/man3/htonl.3p.html)

12. **ntohs**
    - **Usage**: Converts a short integer from network to host byte order.
    - **Link**: [ntohs documentation](https://man7.org/linux/man-pages/man3/ntohs.3p.html)

13. **ntohl**
    - **Usage**: Converts a long integer from network to host byte order.
    - **Link**: [ntohl documentation](https://man7.org/linux/man-pages/man3/ntohl.3p.html)

14. **select**
    - **Usage**: Monitors multiple file descriptors for readiness to perform I/O.
    - **Link**: [select documentation](https://man7.org/linux/man-pages/man2/select.2.html)

15. **poll**
    - **Usage**: Waits for some event on a set of file descriptors.
    - **Link**: [poll documentation](https://man7.org/linux/man-pages/man2/poll.2.html)

16. **epoll (epoll_create, epoll_ctl, epoll_wait)**
    - **Usage**: Provides scalable I/O event notification mechanisms.
    - **Link**: [epoll documentation](https://man7.org/linux/man-pages/man7/epoll.7.html)

17. **kqueue (kqueue, kevent)**
    - **Usage**: Provides a mechanism for event notification on BSD systems.
    - **Link**: [kqueue documentation](https://man.freebsd.org/kqueue.2)

18. **socket**
    - **Usage**: Creates an endpoint for communication.
    - **Link**: [socket documentation](https://man7.org/linux/man-pages/man2/socket.2.html)

19. **accept**
    - **Usage**: Accepts a connection on a socket.
    - **Link**: [accept documentation](https://man7.org/linux/man-pages/man2/accept.2.html)

20. **listen**
    - **Usage**: Marks a socket as passive, indicating it will accept incoming connection requests.
    - **Link**: [listen documentation](https://man7.org/linux/man-pages/man2/listen.2.html)

21. **send**
    - **Usage**: Sends data to a connected socket.
    - **Link**: [send documentation](https://man7.org/linux/man-pages/man2/send.2.html)

22. **recv**
    - **Usage**: Receives data from a connected socket.
    - **Link**: [recv documentation](https://man7.org/linux/man-pages/man2/recv.2.html)

23. **chdir**
    - **Usage**: Changes the current working directory of the calling process.
    - **Link**: [chdir documentation](https://man7.org/linux/man-pages/man2/chdir.2.html)

24. **bind**
    - **Usage**: Associates a socket with an address.
    - **Link**: [bind documentation](https://man7.org/linux/man-pages/man2/bind.2.html)

25. **connect**
    - **Usage**: Initiates a connection on a socket.
    - **Link**: [connect documentation](https://man7.org/linux/man-pages/man2/connect.2.html)

26. **getaddrinfo**
    - **Usage**: Translates a human-readable hostname into a socket address.
    - **Link**: [getaddrinfo documentation](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)

27. **freeaddrinfo**
    - **Usage**: Frees memory allocated by `getaddrinfo()`.
    - **Link**: [freeaddrinfo documentation](https://man7.org/linux/man-pages/man3/freeaddrinfo.3.html)

28. **setsockopt**
    - **Usage**: Sets options on a socket.
    - **Link**: [setsockopt documentation](https://man7.org/linux/man-pages/man2/setsockopt.2.html)

29. **getsockname**
    - **Usage**: Gets the current address to which the socket is bound.
    - **Link**: [getsockname documentation](https://man7.org/linux/man-pages/man2/getsockname.2.html)

30. **getprotobyname**
    - **Usage**: Gets protocol information based on a protocol name.
    - **Link**: [getprotobyname documentation](https://man7.org/linux/man-pages/man3/getprotoent.3.html)

31. **fcntl**
    - **Usage**: Performs various operations on a file descriptor.
    - **Link**: [fcntl documentation](https://man7.org/linux/man-pages/man2/fcntl.2.html)

32. **close**
    - **Usage**: Closes a file descriptor.
    - **Link**: [close documentation](https://man7.org/linux/man-pages/man2/close.2.html)

33. **read**
    - **Usage**: Reads data from a file descriptor.
    - **Link**: [read documentation](https://man7.org/linux/man-pages/man2/read.2.html)

34. **write**
    - **Usage**: Writes data to a file descriptor.
    - **Link**: [write documentation](https://man7.org/linux/man-pages/man2/write.2.html)

35. **waitpid**
    - **Usage**: Waits for a process to change state.
    - **Link**: [waitpid documentation](https://man7.org/linux/man-pages/man2/waitpid.2.html)

36. **kill**
    - **Usage**: Sends a signal to a process.
    - **Link**: [kill documentation](https://man7.org/linux/man-pages/man2/kill.2.html)

37. **signal**
    - **Usage**: Sets a function to handle a specific signal.
    - **Link**: [signal documentation](https://man7.org/linux/man-pages/man2/signal.2.html)

38. **access**
    - **Usage**: Checks the accessibility of a file.
    - **Link**: [access documentation](https://man7.org/linux/man-pages/man2/access.2.html)

39. **stat**
    - **Usage**: Retrieves file status information.
    - **Link**: [stat documentation](https://man7.org/linux/man-pages/man2/stat.2.html)

40. **open**
    - **Usage**: Opens a file and returns a file descriptor.
    - **Link**: [open documentation](https://man7.org/linux/man-pages/man2/open.2.html)

41. **opendir**
    - **Usage**: Opens a directory stream.
    - **Link**: [opendir documentation](https://man7.org/linux/man-pages/man3/opendir.3.html)

42. **readdir**
    - **Usage**: Reads a directory entry from the directory stream.
    - **Link**: [readdir documentation](https://man7.org/linux/man-pages/man3/readdir.3.html)

43. **closedir**
    - **Usage**: Closes a directory stream.
    - **Link**: [closedir documentation](https://man7.org/linux/man-pages/man3/closedir.3.html)