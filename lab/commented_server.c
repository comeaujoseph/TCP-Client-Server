/*
	The steps involved in establishing a socket on the client side are as follows:
		1. Create a socket with the socket() system call
		2. Connect the socket to the address of the server using the connect() system call
		3. Send and receive data. There are a number of ways to do this, but the simplest is to use the read() and write() system calls.
			Note: May want to look into using something other than read() and write()

	The steps involved in establishing a socket on the server side are as follows:
		1. Create a socket with the socket() system call
		2. Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists
			of a port number on the host machine.
		3. Listen for connections with the listen() system call
		4. Accept a connection with the accept() system call. This call typically blocks until a client connects with the server.
		5. Send and receive data
*/

#include <stdlib.h>
#include <stdio.h>		//contains declarations used in most input and output
#include <sys/types.h> 		//contains definitions of a number of data types used in system calls
#include <sys/socket.h>		//includes a number of definitions of structures needed for sockets
#include <netinet/in.h>		//contains constants and structures needed for internet domain addresses
#include <unistd.h>
#include <string.h>

void error(char *msg) {
	perror(msg);
	exit(1);
}

char* ascii_encoder(char *msg) {

}

void connection() {
	
}

int main(int argc, char *argv[]) {
	int c;
	char *pvalue = NULL;
	while ((c = getopt(argc, argv, "p:")) != -1) {
		switch (c) {
			case 'p':
				pvalue = optarg;
				break;
		}
	}

	/*
		sockfd & newsockfd are file descriptors; i.e array subscripts into 
		the file descriptor table.These two variables store the values returned
		by the socket system call and the accept system call.

		What is a file descriptor table????
			Each running process has a file descriptor table which
			contains pointers to all open i/o streams.  When a process
			starts, three entries are created in the first three cells of
			the table.  Entry 0 points to standard input, entry 1 points
			to standard output, and entry 2 points to standard error.
			Whenever a file is opened, a new entry is created in this
			table, usually in the first available empty slot.

			The socket system call returns an entry into this table; i.e.
			a small integer.  This value is used for other calls which
			use this socket.  The accept system call returns another
			entry into this table.  The value returned by accept is used
			for reading and writing to that connection.

		port stores the port number on which the server accepts connections.
		
		clilen stores the size of the address of the client. This is needed for the
		accept system call.

		n is the return value for the read() and write() calls, i.e. it contains the number
		of characters read or written.
	*/
	int sockfd, newsockfd, port, n;
	socklen_t clilen;	

	//the server reads characters from the socket connection into this buffer
	char buffer[256]; 

	/*
		A sockaddr_in is a structure containing an internet address. This 
		structure is defined in <netinet/in.h>. Structure defintion:
			struct sockaddr_in {
			    short   sin_family;
		        u_short sin_port;
		        struct  in_addr sin_addr;
		        char    sin_zero[8];
			};
		An in_addr structure, defined in the same header file, contains only one field, 
		a unsigned long called s_addr. The variable serv_addr will contain the address 
		of the server, and cli_addr will contain the address of the client which 
		connects to the server.
	*/
	struct sockaddr_in serv_addr, cli_addr;

	/*
		The socket() system call creates a new socket. It takes three arguments:
			1. The address domain of the socket. Recall that there are two possible address domains, 
				the unix domain for two processes which share a common file system, and the Internet 
				domain for any two hosts on the Internet. The symbol constant AF_UNIX is used for the
				former, and AF_INET for the latter. 
					AF_UNIX, AF_LOCAL -----> Local communication
					AF_INET ---------------> IPv4 Internet protocols
			2. The type of socket. Recall that there are two choices here, a stream socket in which 
				characters are read in a continuous stream as if from a file or pipe, and a datagram 
				socket, in which messages are read in chunks.
					SOCK_STREAM     Provides sequenced, reliable, two-way, connection-
                       				based byte streams.  An out-of-band data transmission
                       				mechanism may be supported.

			        SOCK_DGRAM      Supports datagrams (connectionless, unreliable
			                       	messages of a fixed maximum length).
			3.	The protocol. If this argument is zero (and it always should be except for unusual 
				circumstances), the operating system will choose the most appropriate protocol. 
				It will choose TCP for stream sockets and UDP for datagram sockets.

		The socket system call returns an entry into the file descriptor table (i.e. a small integer). 
		This value is used for all subsequent references to this socket. If the socket call fails, it returns -1.
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
        error("ERROR opening socket");
	}

	if (pvalue == NULL) {
		port = 51717;
	} else {
		port = atoi(pvalue);
	}
	

	//clear the buffer
	memset(buffer, 0, sizeof(buffer));
	serv_addr.sin_family = AF_INET;
	//The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
	serv_addr.sin_port = htons(port);
	//The value "INADDR_ANY" means that we will bind to any/all IP addresses that the local computer currently has.
	//For server code, this will always be the IP address of the machine on which the server is running, and there
	//is a symbolic constant INADDR_ANY which gets this address.
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	/*
		The listen system call allows the process to listen on the socket for connections. The first argument 
		is the socket file descriptor, and the second is the size of the backlog queue, i.e., the number of 
		connections that can be waiting while the process is handling a particular connection. This should be 
		set to 5, the maximum size permitted by most systems. If the first argument is a valid socket, this 
		call cannot fail, and so the code doesn't check for errors.
	*/
	listen(sockfd,5);

	/*
		The accept() system call causes the process to block until a client connects to the server.
		Thus, it wakes up the process when a connection from a client has been successfully established. 
		It returns a new file descriptor, and all communication on this connection should be done using the new 
		file descriptor. The second argument is a reference pointer to the address of the client on the other end 
		of the connection, and the third argument is the size of this structure.
	*/
	clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
    	error("ERROR on accept");
    }

    /*
    	Note that we would only get to this point after a client has successfully connected to our server. 
    	This code initializes the buffer using the bzero() function, and then reads from the socket. 
    	Note that the read call uses the new file descriptor, the one returned by accept(), not the original
    	file descriptor returned by socket(). Note also that the read() will block until there is something 
    	for it to read in the socket, i.e. after the client has executed a write(). It will read either the 
    	total number of characters in the socket or 255, whichever is less, and return the number of characters read.
    */
    n = read(newsockfd,buffer,255);
    if (n < 0) {
    	error("ERROR reading from socket");
    }
    printf("Here is the message: %s\n",buffer);

    /*
    	Once a connection has been established, both ends can both read and write to the connection. 
    	Naturally, everything written by the client will be read by the server, and everything written by the 
    	server will be read by the client. This code simply writes a short message to the client. The last 
    	argument of write is the size of the message.
    */
	n = write(newsockfd,"I got your message",18);
    if (n < 0) {
    	error("ERROR writing to socket");
    }

    return 0;
}
