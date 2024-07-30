# IRC Server
this repo made to work on the ft_irc project in the cursus of 42 


# Overview 

In a typical client-server scenario, applications communicate using sockets as follows:

* Each application creates a socket. A socket is the "apparatus" that allows communication, and both application require one.

* the server bind its socket to a well-known address (name) so that clients can locate it .

A socket is created using the ```socket()``` system call, which return a file descriptor used to refer to the socket in subsequent system calls:

	fd = socket(domain, type, protocol);

## Communication domains

Sockets exist in a communication domain, which determines:

* the method of identifying a socket (i.e., the format of a socket "address"); and 
* the range of communication (i.e., either between application on the same host or between applications on different hosts connected via a network).

Modern operating systems support at least the following domains:

* the UNIX (AF_UNIX) domain allows communication between applications on the same host. (POSIX.1g used the name AF_LOCAL as a synonym for AF_UNIX , but this is not used in SUSv3.)

* The IPv4 (AF_INET) domain allows communication between applications running on hosts connected via an Internet Protocol version 4 (IPv4) network.

* The IPv6 (AF_INET6) domain allows communication between applications running
on hosts connected via an Internet Protocol version 6 (IPv6) network.
Although IPv6 is designed as the successor to IPv4, the latter protocol is currently still the most widely used.


<table style="margin-left: auto; margin-right: auto;">
	<caption> Socket domains </caption>
	<tr>
		<th>Domain</th>
		<th>Communication performed</th>
		<th>Communication between applications</th>
		<th>Address format</th>
		<th>Address structure</th>
	</tr>
	<tr>
		<td>AF_UNIX</td>
		<td>within kernel</td>
		<td>on same host</td>
		<td>pathname</td>
		<td>sockaddr_un</td>
	</tr>
	<tr>
		<td>AF_INET</td>
		<td>via IPv4</td>
		<td>on hosts connected via an IPv4 network</td>
		<td>32-bit IPv4 address + 16-bit port number</td>
		<td>sockaddr_in</td>
	</tr>
	<tr>
		<td>AF_INET6</td>
		<td>via IPv6</td>
		<td>on hosts connected via an IPv6 network</td>
		<td>128-bit IPv6 address + 16-bit port number</td>
		<td>sockaddr_in6</td>
	</tr>
</table>

## Socket types

Every sockets implementation provides at least two types of sockets: stream and datagram. These socket types are supported in both th UNIX and the Internet domains. the next table summarizes the properties of these cocket types.

<table style="width:100%">
<caption> Socket types and their properties </caption>
	<tr>
		<th rowspan="2"> Propery </th>
		<th colspan="2"> Socket type</th>
	</tr>
	<tr>
		<th> Stream </th> 
		<th> Datagram </th> 
	</tr>
	<tr>
		<td>Reliable delivery?</td>
		<td>Y</td>
		<td>N</td>
	</tr>
	<tr>
		<td>Message boundaries preserved?</td>
		<td>N</td>
		<td>Y</td>
	</tr>
	<tr>
		<td>Connection-oriented?</td>
		<td>Y</td>
		<td>N</td>
	</tr>
</table>

### Stream sockets (SOCK_STREAM) provide a reliable, bidirectional, byte-stream communication channel. By the terms in this description, we mean the following:
* <b>Reliable</b> means that we are guaranteed that either the transmitted data will arrive intact at the receiving application, exactly as it was transmitted by the sender (assuming that neither the network link nor the receiver crashes), or that we’ll receive notification of a probable failure in transmission.

* <b>Bidirectional</b> means that data may be transmitted in either direction between two sockets.

* <b>Byte-stream</b> means that, as with pipes, there is no concept of message boundaries

A stream socket is similar to using a pair of pipes to allow bidirectional communication between two applications, with the difference that (internet domain) sockets permit communication over a network.

Stream sockets operate in connected pairs. For this reason, stream sockets are described as ```connection-oriented```.

### Datagram sockets (SOCK_DGRAM) allow dadta to be exchanged in the form of messages called ```datagrams```.

with datagram sockets, message boundaries are preserved, but data transmission is not reliable. Messages may arrive out of order, be duplicated, or not arrive at all.
Datagram sockets are an example of the more generic concept of a <b>```connectionless```</b> socket. Unlike a stream socket, a datagram socket doesn't need to be connected to another socket in order to be used.
In the Internet domain, datagram sockets employ the User Datagram Protocol (UDP), and stream sockets (usually) employ the Transmission Control Protocol (TCP).

## Socket system calls:

The key socket system calls are the following:

* The <i>socket()</i> : creates a new socket.
* the <i>bind()</i> : binds a socket to an address. Usually, a server employs this call to bind its socket to a well-known address so that clients can locate the socket.
* The <i>listen()</i> : allows a stream socket to accept incomming connections from other sockets.
* The <i>accept()</i> : accepts a connection from a peer application on a listening stream socket, and optionally returns the address of the peer socket.
* The <i>connect()</i> : establishes a connection with another socket.

Socket I/O can be performed using the conventional read() and write() system calls, or using a range of socket-specific system calls (e.g., ```send()```, ``recv()``, `sendto()`, and ````recvfrom()````). By default, these system calls block if the I/O operation can't be completed immediately. Nonblocking I/O is also possible, by using the ```fcntl()``` F_SETFL operation to enable the O_NONBLOCK open file status flag.

# Creating a Socket: socket()

the socket() system call creates a new socket.

	int socket(int domain, int type, int protocol);

* * domain   : specifies the communication domain for the socket (e.g, AF_UNIX, AF_INET, AF_INET6 ...)
* * type     : specifies the socket type (e.g, SOCK_STREM, SOCK_DGRAM...)
* * protocol : non-zero protocol is specified as IPPROTO_RAW for raw sockets (SOCK_RAW).

* return : returns a file descriptor used to refer to the newly created socket ,or -1 on error

	starting with kernel 2.6.27, linux provides a second use for the ``type`` argument, by allowing two nonstandard flags to be ORed with the socket type. The <i><b>SOCK_CLOEXEC</b></i> flag causes the kernel to enable the close-on-exec flag (FD_CLOEXEC) for new file descriptor. This flag is useful for the same reasons as the open() O_CLOEXEC flag . The <i><b>SOCK_NONBLOCK</b></i> flag causes the kernel to set the O_NONBLOCK flag on the underlying open file description, so that future I/O operations on the socket will be nonblocking. This saves additional calls to ```fcntl()``` to acheve the same result.

# binding a Socket to an address: bind()

the bind() system call binds a socket to an address.

	int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

* * sockfd    : is a file descriptor obtained from the previous call to socket().
* * addr      : is a pointer to a structure specifying the address to which this socket is to be bound. the type of structure passed in this argument depends on the socket domain.
* * addrlen   : specifies the size of the address structure. The ```socklen_t``` data type used for the addrlen argument is an integer type specified by SUSv3.

* return : returns 0 on success, or –1 on error

typecally, we bind a server's socket to a well-known address-that is, a fixed address that is known in advance to client applications that need to communicate with that server.

# Generic Socket Address Structures: struct sockaddr

The <i>addr</i> and <i>addrlen</i> arguments to `bind()` require some furher explanation. Looking to the table [above](#communication-domains) we see that each socket domain uses a different address format.
For exemple, UNIX domain sockets use pathnames, while Internet domain sockets use the combination of an IP address plus a port number. For each socket domain, a different structure type is defined to store a socket address. However, because system calls such as bind() are generic to all socket domains, they must be able to accept address structures of any type. In order to permit this, the socket API defines a generic address structure, <b>struct sockaddr</b>. The only purpose for this type is to cast the various domain-specific address structures to a single type for use as arguments in the socket system calls. The <b> sockaddr</b> structure is typically defined as follows:

	struct sockaddr {
		sa_family_t sa_family;   /*Address family (AF_* constant)*/
		char        sa_data[14]; /*Socket address (size varies according to socket domain) */
	};

this structure serves as a template for all of the domain-specific address structures. 
Each of these address structures begins with a <i>family</i> field corresponding to the <i> sa_family</i> field of the <i> sockaddr</i> structure.(The <i> sa_family_t</i> data type is an integer type specified in SUSv3.) the value in the <i> family</i> field is sufficient to determine the size and format of the address stored in the remainder of the structure.

<table bgcolor="gray">
	<tr>
		<td>
				Some UNIX implementations also define an additional field in the   <mark> sockaddr</mark> structure, <mark> sa_len</mark>, that specifies the total size of the structure. SUSv3 doesn't require this field, and it is not present in the linux implementation of the sockets API.
		</td>
	</tr>
</table>

# Stream Sockets

The operation of stream sockets can be explained by anology with the telephone system: 

1. The <i> socket() </i> system call, which creates a socket, is the equivalent of installing a telephone. In order for two application to communicate, each of them must create a socket.
2. Communication via a stream socket is analogous to a telephone call. One application must connect its socket to another application's socket before communication can take place. Two sockets are connected  as follows:
	1. One application calls `bind()` in order to bind the socket to a well-known address, and then calls `listen()` to notify the kernel of its willingness to accept incoming connections. This step is analogous to having a known telephone number and ensuring that out telephone is turned on so that people can call us.
	2. The other application establishes the connection by calling `connect()`, specifying the address of the socket to which the connection isto be made. This is analogous to dialing someone's telephone number.
	3. The application that called `listen()` then accepts the connection using `accept()`. This is analogous to picking up the telephone when it rings. If the `accept()` is performed before the peer application calls `connect() `, then the `accept()` <mark> <b> blocks (waiting by the telephone) </b> </mark>.
3. Once a connection has been established, data can be transmitted in both directions between the applications (analogous to a two-way telephone conversation) until one of them closes the connection using `close()`. Communication is performed using the conventional <i> read()</i> and <i> write</i> system calls or via a number of socket specific system calls (such as <i> send()</i> and <i>recv()</i>) that provide additional functionality.



## Active sockets (client) and passive sockets (server) :

Stream sockets are often distinguished as being either active or passive:

* by default, a socket that has been created using `socket()` is active. An active socket can be used in a `connect()` call to establish a connection to a passive socket. This is referred to as performing an <mark> active open </mark>.
* A passive sicket (also called a <i>listening</i> socket) is one that has been marked to allow incoming connections by calling <i>listen</i>. Accepting an incoming connection is referred to as perfoming a passive open.

In most applications that employ stream sockets, the server performs the passive open, and the client performs the active open.

![Overview of system calls used with stream sockets](README_utils/socket_api_client_server.png)

# Listening for Incoming Connections: listen()

The <i>listen()</i> system call marks the stream socket referred to by the file descriptor <i>sock_fd</i> as <b> passive</b>. The socket will subsequently be used to accept connections from other (active) sockets.

	int listen(int sockfd, int backlog);

* * sockfd    : is a file descriptor obtained from the previous call to socket().
* * backlog   : allows us to limit the number of pending connections. Connection requests up to this limit succeed immediately.

* return 	  : return 0 on success, or -1 on error.

We can’t apply `listen()` to a connected socket—that is, a socket on which a `connect()` has been successfully performed or a socket returned by a call to accept().

To understand the purpose of the <i>backlog</i> argument, we first observe that the client may call `connect()` before the server calls `accept()`. This could happen, for example, because the server is busy handling some other client(s). This results in a <i> pending connection </i>.

![A pending socket connection](README_utils/pending_socket_connection.png)


The kernel must record some information about each pending connection request so that a subsequent <i>accept()</i> can be processed. The <i>backlog</i> argument allows us to limit the number of such pending connections. Connection requests up to this limit succeed immediately. (For TCP sockets, the story is a little different) Further connection requests block until a pending connection is accepted (via `accept()`), and thus removed from the queue of pending connections.

# Accepting a Connection: accept()

the <i> accept() </i> system call accepts an incoming connection on the linstening stream socket referred to by the file descriptor <i>sockfd</i>. If there are no pending connections when <i>accept()</i> is called, the call blocks until a connection request arrives.

	int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

The key point to understand about <i> accept() </i> is that it creates n <i> new </i> socket, and it is this new socket that is connected to the peer socket that performed the <i>connect()</i>. A file descriptor for the connected socket is returned as the function result of the <i> accept() </i> call. The listening socket (<i> sockfd</i>) remains open, and can be used to accept further connections. A typical server application creates one listening socket, binds it to a well-known address, and then handles all client requests by accepting connection via that socket.

The remainnig arguments to <i> accept()</i> return the address of the peer socket (client socket) . The <b> addr</b> argument points to a structure that is used to return the socket address. The type of this argument depends on the socket domain (as for <i> bind()</i>).
The <b> addrlen</b> argument is a value-result argument. It points to an integer that, proir to the call, must be initialized to the size of the buffer pointed to by <i> addr</i>, so that the kernel knows how much space is available to return the socket address.

Upon return from <i>accept()</i>, this integer is set to indicate the number of bytes of data actually copied into the buffer.

<table bgcolor="gray">
	<tr>
		<td>
				If we are not interested in the address of the peer socket, then <i> addr</i> and <i> addrlen</i> should be specified as NULL and 0, respectively. (If desired, we can retrieve the peer's address later using the <b><i> getpeername()</i></b> system call).
		</td>
	</tr>
</table>

# Connecting to a Peer Socket: connect()

The <i> connect()</i> system call connects the active socket referred to by  the file descriptor <i>sockfd</i> to the listening socket whose address is specified by <i>addr</i> and <i> addrlen</i> .

	int connect(int sockfd, cont struct sockaddr *addr, socklen_t addrlen);
	return 0 on success, or -1 on error.


the <i> addr </i> and <i> addrlen</i> arguments are specified in the same way as the  corresponding arguments to `bind()`.

<table bgcolor="gray">
	<tr>
		<td>
				If <i> connect() </i> fails and we wish to reattempt the connection, then SUSv3 specifies that the portable method of doing so is to close the socket, create a new socket, and reattempt the connection with the new socket.
		</td>
	</tr>
</table>

# I/O Strem Sockets

A pair of connected stream sockets provides a bidirectional communication channel between the two endpoints.

The next figure shows what this looks like in the UNIX domain.

![UNIX domain stream sockets provide a bidirectional communication channel](README_utils/unix_domain_stream_sockets.png)

The semantics of I/O on connected stream sockets are similar to those for pipes:

* To perform I/O, we use the <i> read() </i> and <i> write()</i> system calls (or the socket-specific <i> send() </i> and <i> recv()</i>). Since sockets are bidirectional, both calls may be used on each end of the connection.
* A socket may be closed using the <i>close()</i> system call or as a consequence of the application terminating. Afterward, when the peer application attempts to read from the other end of the connection, it recieves end-of-file (once all buffread data has been read). If the peer application attempts to write to its socket, it receives a <b> SIGPIPE</b> signal, and the system call fails with the error <b> EPIPE</b>. the usual way of dealing with this possibility is to ignore the <b> SIGPIPE</b> signal and find out about the closed connection via the <b>EPIPE</b> error.


# Connection Termination: close()

The usual way of terminating a stream socket connection is to call `close()`. If multiple file descriptors refer to the same socket, then the connection is terminated when all of the descriptors are closed.

Suppose that, after we close a connection, the peer application crashes or other- wise fails to read or correctly process the data that we previously sent to it. In this case, we have no way of knowing that an error occurred. If we need to ensure that the data was successfully read and processed, then we must build some type of acknowledgement protocol into our application. This normally consists of an explicit acknowledgement message passed back to us from the peer.

# I/O Muntiplexing

I/O multiplexing allows us to simultaneously monitor multiple file descriptors to see if I/O is possible on any of them. We can perform I/O multiplexing using either of two system calls with essentially the same functionality. The first of  these, <i> select()</i>, appeared along with the sockets API in BSD. This was historically the more  widespread of the two system calls. The other system call, <i> poll()</i>, appeared in System V. Both <i> select() </i> and <i> poll() </i> are nowadays required by SUSv3.

We can use <i> select()</i> and <i> poll()</i> to monitor file descriptors for regular files, terminals, pseudoterminals, pipes, FIFOs, sockets, and some types of character devices. 
<br>Both system calls allow a process either to block indefinitely waiting for the file descriptors to become ready or to specify a timeout on the call.

## The <i> select()</i> System Call

The `select()` system call blocks until one or more of a set of file descriptors becomes ready.

```
	#include <sys/time.h>
	#include <sys/select.h>
	int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
	Returns number of ready file descriptors, 0 on timeout, or –1 on error
```

The <i> nfds, readfds, writefds, </i> and <i> exceptfds</i> arguments specify the file descriptors that `select()` is to monitor. the <i> timeout </i> argument can be used to set an upper limit on the time for whiche select() will block.

### File descriptor sets 

The <i> readfds, writefds, </i>and <i> exceptfds </i> argumets are pointers to **file descriptor ** sets, represented using the data type <b> fd_set</b> . These arguments are used as follows :

* <i> readfds</i>   : is the set of file descriptors to be tested to see if input is possible;
* <i> writefds</i>  : is the set of file descriptors to be tested to see if output is possible;
* <i> exceptfds</i> : is the set of file descriptors to be tested to see if an exceptional condition has occurred.

The term <i> exceptional condition</i> is often misunderstood to mean that some sort of error condition has arisen on the file descriptor. This is not the case. An exceptional condition occurs in just two circumstances on Linux(other UNIX implementations are similar):

* A state change occurs on a pseudoterminal slave connected to a master that is in packet mode
* Out-of-band data is received on a stream socket

Typically, the <i> fd_set</i> data type is implemented as a bit mask. However, we don't need to know the details, since all manipulation of file descriptor sets is done via four macros: FD_ZERO(), FD_SET(), FD_CLR(), and FD_ISSET().

```
	#include <sys/select.h>
	void FD_ZERO(fd_set *fdset);
	void FD_SET(int fd, fd_set *fdset);
	void FD_CLR(int fd, fd_set *fdset);

	int FD_ISSET(int fd, fd_set *fdset);
		Returns true (1) if fd is in fdset, or false (0) otherwise
```

These macros operate as follows:

* FD_ZERO() initializes the set pointed to by <i> fdset</i>  to be empty.
* FD_SET()  adds the file descriptor <i> fd</i> to the set pointed to by <i> fdset</i>.
* FD_CLR()  removes the file descriptor <i> fd</i> from the set pointed to by <i> fdset</i> 
* FD_ISSET() returns true if the file descriptor <i> fd</i> is a member of the set pointed to by <i> fdset</i>.

a file descriptor set has a maximum size, defined by the constant FD_SETSIZE. On Linux, this constant has the value 1024.

The **readfds**, **writefds**, and **exceptfds** arguments are all value-result. Befor the call to `select()`, the **fd_set** structures pointed to by these arguments must be initialized (using FD_ZERO() and FD_SET()) to contain the set of file descriptors of interest. The `select()` call modifies each of these structures so that, on return, they contain the set of file descriptors that are ready. (Since these structures are modified by the call, we must ensure that we reinitialize them if we are repeatedly calling `select()` from within a loop.) The structures can then be examined using FD_ISSET().

If we are not interested in a particular class of events, then the corresponding fd_set argument can be specified as NULL.

### the <i> timeout </i> argument

The <i> timeout </i> argument controls the blocking behavior of select(). It can bespecified either as NULL, in which casse <i> select()</i> indefinitely, or as a pointer to a <i> timeval </i> structure: 

```
	struct timeval {
		time_t		tv_sec;					/*Seconds*/
		suseconds_t	tv_usec;				/*Microseconds (long int ) */
	};
```

If both fields of <i> timeout </i> are 0, then <i> select()</i> doesn't block; it simply polls the specified file descriptors to see which ones are ready and returns immediately. Otherwise, <i> timeout </i> specifies an upper limit on the time for which <i> select()</i> is to wait.

Although the <i> timeval</i> structure affords mmicrosecond precision, the accuracy of the call is limited by the granularity of the software clock.

when timeout is Null, or points to a structure containing nonzero fields, <i> select()</i> blocks until one of the following occurs:

* at least one of the file descriptors specified in <i> readfds, writefds, </i> or <i> exceptfds </i>becomes ready;
* the call is interrupted by a signal handleer;
* the amount of time specified by timeout has passed.

### Return value from <i> select()</i>

As its function result, <i>select() </i> return one of the following:

* A return value of -1 indicates that an error occurred. Possible errors include EBADF and EINTR. 
* * EBADF indecates that one of the file descriptors in  *readfds, writefds,* or *exceptfds* is invalid (e.g., not currently open).
* * EINTR indecates that the call was interrupted by a singnal handler.
* A return value of 0 means that the call timed out before any file descriptor became ready .In this case each of the returned file descriptor sets will be empty.
* a positive return value indicates that one or more file descriptors is ready. The return value in the number of ready descriptors. In this case, each of the returned file descriptor sets must be examined (using FD_ISSET()) in order to find out which I/O events occurred. If the same file descriptor is specified in more than one of *readfds, writefds,* and *exceptfds* ,it is counted multiple times if it is ready for more than one event. In other words, *select()* returns the total number of file descriptors marked as ready in all three returned sets.

## The *poll()* System Call 

The *poll()* system call performs a similar task to *select()*. the major difference between the two system calls lies in how we specify the file descriptors to be monitored. With *select()*, we provide three sets, each marked to indicate the file descriptors of interest. With *poll()*, we provide a list of file descriptors, each marked with the set of events of interest.

```
	#include <poll.h>
	int poll(struct pollfd fds[], nfds_t nfds, int timeout);
			Returns number of ready file descriiptors, 0 on timeout, or -1 or error
```

The *fds* argument and the *pollfd* array (nfds) specify the file descriptors that *poll()* is to monitor. The *timeout* argument can be used to set an upper limit on the time for which *poll()* will block. We describe each of these arguments in detail below.

### the *pollfd* array

The *fds* argument lists the file descriptors to be monitored by *poll()*. This argument is an array of *pollfd* structures, defined as follows:

```
	struct pollfd {
        int   fd;				/* File descriptor */
        short events;			/* Requested events bit mask */
        short revents;			/* Returned events bit mask */
	};
```

The *nfds* arguments specifies the number of items in the fds array. The *nfds_t* data type used to type the *nfds* argument is an **unsigned integer** type.

The *events* and *revents* fields of the pollfd structure are bit masks, <mark>The caller initializes *events* to specify the events to be monitored for the file descriptor *fd*</mark>. Upon return from *poll()*, <mark>*revents* is set to indicate which of those events actually occurred for this file descriptor</mark>.

Table Bellow lists the bits that may appear in the *events* and *revents* fields.

The first group of bits in this table (POLLIN, POLLRDNORM, POLLRDBAND, POLLPRI, and POLLRDHUP) are concerned with input events.

The next group of bits (POLLOUT, POLLWRNORM, and POLLWRBAND) are concerned with output events.

the third group of bits (POLLERR, POLLHUP , and POLLNVAL) are set in the *revents* field to return additional information about the file descriptor. If specified in the *events* field, these three bits are ignored.

The final bit (POLLMSG) is unsused by *poll()* on linux.

<table style="margin-left: auto; margin-right: auto;">
	<caption> Bit-mask values for events and revents fields of the pollfd structure</caption>
	<tr>
		<th>Bit</th>
		<th>Input in *events*?</th>
		<th>Return in *revents*</th>
		<th>Description</th>
	</tr>
	<tr>
		<td>POLLIN</td>
		<td rowspan="2">*</td>
		<td rowspan="2">*</td>
		<td>Data other than high-priority data can be read</td>
	</tr>
	<tr>
		<td>POLLRDNORM</td>
		<!-- <td>*</td>
		<td>*</td> -->
		<td>Equivalent to POLLIN</td>
	</tr>
	<tr>
		<td>POLLRDBAND</td>
		<td>*</td>
		<td>*</td>
		<td>Priority data can be read (unused on Linux)</td>
	</tr>
	<tr>
		<td>POLLPRI</td>
		<td>*</td>
		<td>*</td>
		<td>High-priority data can be read</td>
	</tr>
	<tr>
		<td>POLLRDHUP</td>
		<td>*</td>
		<td>*</td>
		<td>Shutdown on peer socket</td>
	</tr>
	<tr>
		<td>POLLOUT</td>
		<td rowspan="2">*</td>
		<td rowspan="2">*</td>
		<td>Normal data can be written</td>
	</tr>
	<tr>
		<td>POLLWRNORM</td>
		<!-- <td>*</td>
		<td>*</td> -->
		<td>Equivalent to POLLOUT</td>
	</tr>
	<tr>
		<td>POLLWRBAND</td>
		<td>*</td>
		<td>*</td>
		<td>Priority data can be written</td>
	</tr>
	<tr>
		<td>POLLERR</td>
		<td>   </td>
		<td>*</td>
		<td>An error has occurred</td>
	</tr>
	<tr>
		<td>POLLHUP</td>
		<td>   </td>
		<td>*</td>
		<td>A hangup has occurred</td>
	</tr>
	<tr>
		<td>POLLNVAL</td>
		<td>   </td>
		<td>*</td>
		<td>File descriptor is not open</td>
	</tr>
	<tr>
		<td>POLLMSG</td>
		<td>  </td>
		<td>  </td>
		<td>Unused on Linux (and unspecified in SUSv3)</td>
	</tr>
</table>

### the *timeout* argument:

The *timeout* argument determines the blocking behavoir of *poll()* as follows:

* case 1: *timeout = -1* : block until one of the file descriptors listed in the *fds* array is ready (as defined by the corresponging *events* field) or a signal is caught.
* case 2: *timeout = 0*  : do not block - just perform a check to see which file descriptors are ready.
* case 3: *timeout > 0*  : block for up to *timeout* milliseconds, until one of the file descriptors in *fds* is ready, or until a signal is caught.

As with *select()*, the accuracy of *timeout* is limited by the granularity of the software clock.

### Return value from *poll()*

As its function result, poll() returns one of the following :
* -1  : indicates that an error occurred. One possible error is EINTR, indicating taht the call was interrupted by a signal handler. 
* 0   : means that the call timed out before any file descriptor became ready.
* \> 0 : indicates that one or more file descriptors are ready. The returned value is the number of *pollfd* structures in the *fds*
 array that have a nonzero *revents* field .

<table bgcolor="gray">
	<tr>
		<td>
			Note the slightly different meaning of a positive return value from <i>select()</i> and <i>poll()</i>. The <i>select()</i> system call counts a file descriptor multiple times if it occurs in more than one returned file descriptor set. The <i>poll()</i> system call returns a count of ready file descriptors, and a file descriptor is counted only once, even if multiple bits are set in the corresponding <i>revents</i> field.
		</td>
	</tr>
</table>

