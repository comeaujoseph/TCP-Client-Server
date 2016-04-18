#include <stdlib.h>
#include <stdio.h>			//contains declarations used in most input and output
#include <sys/types.h> 		//contains definitions of a number of data types used in system calls
#include <sys/socket.h>		//includes a number of definitions of structures needed for sockets
#include <netinet/in.h>		//contains constants and structures needed for internet domain addresses
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

/*
	This function is called when a system call fails. 
	It displays a message about the error on stderr and 
	then aborts the program.
*/
void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char* argv[]) {
	int sockfd, portno, n;
	
	//serv_addr will contain the address of the server to which you want to connect
	struct sockaddr_in serv_addr;

	/*
		The hostent structure is used by functions to store information about a 
		given host, such as host name, IPv4 address, and so forth.
		
		This structure is defined in the header file netdb.h as follows:
			struct  hostent {
			        char    *h_name;        // official name of host
			        char    **h_aliases;    // alias list
			        int     h_addrtype;     // host address type 
			        int     h_length;       // length of address 
			        char    **h_addr_list;  // list of addresses from name server 
			#define h_addr  h_addr_list[0]  // address, for backward compatiblity 
			};

		It defines a host computer on the Internet. The members of this structure are:
			h_name       Official name of the host.

			h_aliases    A zero  terminated  array  of  alternate
			             names for the host.

			h_addrtype   The  type  of  address  being  returned;
			             currently always AF_INET.

			h_length     The length, in bytes, of the address.

			h_addr_list  A pointer to a list of network addresses
			             for the named host.  Host addresses are
			             returned in network byte order.This is the vector 
			             of addresses for the host. (Recall that the 
			             host might be connected to multiple networks 
			             and have different addresses on each one.) 
			             The vector is terminated by a null pointer.

			h_addr 		 This is a synonym for h_addr_list[0]; in other words, it is the first host address.

			Note that h_addr is an alias for the first address in the array of network addresses.
	*/
	struct hostent *server;

	char buffer[256];
	//clear the buffer
	memset(buffer, 0, sizeof(buffer));

	portno = 55555;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
        error("ERROR opening socket");
	}

	/*
		struct hostent *gethostbyname(char *name)
		Takes such a name as an argument and returns a pointer to a hostent containing information 
		about that host. The field char *h_addr contains the IP address. If this structure is NULL, 
		the system could not locate a host with this name.
	*/
	server = gethostbyname("sdsu");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    //memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);
    
    //Convert IP address into inet address stored in sockaddr
  	inet_aton("192.168.1.217", &serv_addr.sin_addr);

    /*
    	The connect function is called by the client to establish a connection to the server. 
    	It takes three arguments, the socket file descriptor, the address of the host to which 
    	it wants to connect (including the port number), and the size of this address. This 
    	function returns 0 on success and -1 if it fails.

    	Notice that the client needs to know the port number of the server, but it does not need 
    	to know its own port number. This is typically assigned by the system when connect is called.
    */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    printf("Please enter the message: ");
    fgets(buffer,255,stdin);
    printf("%s\n", buffer);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) {
         error("ERROR writing to socket");
    }
    memset(buffer, 0, sizeof(buffer));
    n = read(sockfd,buffer,255);
    if (n < 0) {
         error("ERROR reading from socket");
    }
    printf("%s\n",buffer);

	return 0;
}
