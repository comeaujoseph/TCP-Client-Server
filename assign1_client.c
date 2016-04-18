#include <stdlib.h>
#include <stdio.h>			
#include <sys/types.h> 		
#include <sys/socket.h>		
#include <netinet/in.h>		
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT 256
#define DEFAULT_IP "192.168.1.217"
#define DEFAULT_PORT 55555

void error(char *msg) {
    fprintf(stderr, "%s\n", msg);
	exit(1);
}

/*
    Responsible for processing user input, storing it in a char array. This function should throw
    an error if the user (1) doesn't enter any text or (2) enters text that exceeds 256 characters.
    Design decisions was to include the newline character as part of the 256 character limit, so 
    technically the user has 255 characters pluse the carriage return.
*/
char* user_input(char* buffer) {
    char c;
    int count = 0;

    printf("Enter your text message: ");
    c = getchar();
    while((c != '\n')) {
        if (count >= MAX_INPUT - 1) {
            error("ERROR: text message greater than 256 characters.");
        }
        buffer[count++] = c;
        c = getchar();
    }

    if (!count) {
        error("ERROR: no valid text message provided.");
    }
    return buffer;
}

/*
    This function creates a socket that will be used to communicate with the server machine. 
    It requires two arguments; host which the ip address of the server machine, and port which
    is the port number that the server socket is on. After successfully connecting to the server,
    the user enters a message that is then sent to the server. If the message is successfully sent,
    it waits for a response from the server. If a response is recieved error free, then it gets
    displayed on the console.
*/
void connection(char* host, int port) {
    int sockfd, n;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR: create an endpoint for communication.");
    }

    //holds the address and port number of the server machine
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &(serv_addr.sin_addr));

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        fprintf(stderr, "ERROR: connecting to socket %s:%d.\n", host, port);
        exit(1);
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    char* msg = user_input(buffer);
    n = send(sockfd, msg, strlen(msg), 0);
    if (n < 0) {
        error("ERROR: sending a message on the socket.");
    }
    

    memset(buffer, 0, sizeof(buffer));
    n = recv(sockfd, buffer, MAX_INPUT, 0);
    if (n < 0) {
        error("ERROR: receiving a message from socket.");
    }
    printf("%s\n",buffer);

    close(sockfd);
}

/*
    Parses the command line arguments. The user can pass in two types of
    arguments; (1) server's IP address and (2) server's port number that the
    socket is on. If no IP or Port number is provided then it will use the 
    default ip address and port number, which is based on my test environment.
    It will then call connection() passing in the appropriate ip and port.
*/
int main(int argc, char* argv[]) {
    int c;
    char *pvalue = NULL;
    char *ivalue = NULL;
    while ((c = getopt(argc, argv, "p:i:h")) != -1) {
        switch (c) {
            case 'p':
                pvalue = optarg;
                break;
            case 'i':
                ivalue = optarg;
                break;
            case 'h':
               printf("usage:\n"          
                "   -s   with argument: server ip address (default ip = 192.168.1.217)\n"
                "   -p   with argument: server port number (default port = 55555)\n"
                "   -h   Help Menu\n");
               exit(0);
        }
    }
	
    int port;
    if (pvalue == NULL) {
        port = DEFAULT_PORT;
    } else {
        port = atoi(pvalue);
    }
    if (ivalue == NULL)
    {
        ivalue = DEFAULT_IP;
    }

    printf("Using ip address: %s and port: %d\n", ivalue, port);

    connection(ivalue, port);


	return 0;
}
