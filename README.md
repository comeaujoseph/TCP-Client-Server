------------------------------------------------
Author: 		Joey Comeau
Assignment: Programming Assign 1
Date: 			February 22, 2016

Languages: 		C
Platforms:		Mac OS X, Ubuntu
------------------------------------------------

SUMMARY:
	The goal is to implement both a TCP server and a TCP client piece of software. The server 
	is to accept a connection from a client and receive a text message of no more than 256 characters.
	It will convert the message to an encoded message as detailed next. Your server will convert 
	each character by replacing it with the next character in the ASCII sequence. The client will connect
	to the server using the same port and then pass a message to the server. The server will respond 
	with the message converted to the encoded string. The client will then display the converted message 
	to the monitor.

MANUAL:
	In addition to the source code (assign1_client.c and assign_server.c), a Makefile is provided which will 
	generate two executables:
		(1) server
		(2) client
	Note: It is preferrable to run the server and client scripts on two different machines.
	Steps:
		1. 	In the same directory as the source code/Makfile run...
				make
			It should compile without any warnings/errors

		2.	Now start up the server script using the default settings, i.e. port 55555...
				./server
			You can specify a certain port to create a socket on...
				./server -p PORT_NUM

		3. 	Now start up the client script using the default settings, i.e. IP address 192.168.1.217 and port 55555...
				./client
			You can specify a certain port or ip address by doing the following...
				./client -s IP_ADD -p PORT_NUM
			By running the following command you can get a help menu that display optional flags you can use...
				./client -h

			Note: The IP address must be the IP address of the machine that the server script is running on. Also, the port
			number provided must the same port number that the server created the socket on. The default values are based on 
			the test environment I setup to ensure that the scripts were working correctly.

		4. 	Once the client has successfully connected to the server, the user will be prompted to type a message that will
			be sent to the server

		5. 	After entering a message, the server will recieve and encode the client message. Then send back the encoded message
			and the client will then print to the console


