#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT 256
#define DEFAULT_PORT 55555

void error(char *msg) {
    fprintf(stderr, "%s\n", msg);
	exit(1);
}

/*
	Takes one argument, char* msg, which holds the message that
	was sent from the client machine. This function iterates through
	the pointer one character at a time and replacing it with the 
	next character in the ASCII sequence. It then returns the encoded
	message.
*/
char* ascii_encoder(char *msg) {
	int length = strlen(msg);
	int i = 0;
	while (i < length) {
		int bump = (int)msg[i];
		bump++;
		msg[i] = (char)bump;
		i++;
	}
	return msg;
}

/*
	This function handles incoming and outgoing messages from the client 
	machine. It calls recv() which blocks until it gets data from the client, 
	storing it in a char array. Once the data is received, the message gets 
	encoded and sent back to the client. 
*/
void data_transfer(int newsockfd) {
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));

	int n = recv(newsockfd, buffer, MAX_INPUT, 0);
    if (n < 0) {
    	error("ERROR: receiving a message from socket.");
    }

    char* encoded = ascii_encoder(buffer);

	n = send(newsockfd, encoded, strlen(encoded), 0);

    if (n < 0) {
    	error("ERROR: sending a message on the socket.");
    }
}

/*
	This function creates a socket that will be used to communicate with the client machine.
	It then binds this socket to an specific address, which consists of an ip address and a
	port number. It then uses the listen call, which defines how many connections can be pending
	on the specified socket. The accept call causes the thread to block until a client has connected.
	It then calls data_transfer() which handles receiving and sending data. 
*/
void connection(int port) {
	int yes = 1;

	int sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
        error("ERROR: create an endpoint for communication.");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            error("ERROR: refreshing socket."); 
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR: binding name to socket.");
	}

	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
    	error("ERROR: accepting a connection on the socket.");
    }

    data_transfer(newsockfd);
    close(newsockfd);
    close(sockfd);
}

/*
    Parses the command line arguments. The user can pass in a port number to be used
    by using the -p flag. If no Port number is provided then it will use the 
    default port number, which is based on my test environment. It will then call
    connection() passing in the appropriate port number.
*/
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

	int port;
	if (pvalue == NULL) {
		port = DEFAULT_PORT;
	} else {
		port = atoi(pvalue);
	}

	printf("Using port: %d\n", port);
	connection(port);

    return 0;
}
