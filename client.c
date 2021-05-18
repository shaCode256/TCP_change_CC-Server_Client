#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#define MAX 80
#define PORT_SERVER 8080
#define SA struct sockaddr
#define SIZE 1024
#define MAX 80
#define PORT 8080
#define SIZE 1024

void send_file(FILE *fp, int sockfd)
{
	char data[SIZE] = {0};
	while (fgets(data, SIZE, fp) != NULL)
	{
		if (send(sockfd, data, sizeof(data), 0) == -1)
		{
			perror(" There's an ERROR in sending the file ");
			exit(1);
		}
		bzero(data, SIZE);
	}
}

void mySend(int sockfd)
{
	for (int i = 0; i < 5; i++)
	{
		FILE *fp = fopen("ourFile.txt", "rb");
		if (fp == NULL)
		{
			perror(" There's an ERROR in reading the file ");
			exit(1);
		}
		else
		{
			send_file(fp, sockfd);
			fclose(fp);
			fp = NULL;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf(" ERROR creating the socket \n");
		exit(0);
	}
	else
	{
		printf(" Creating the socket was successfully done \n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = PORT_SERVER;

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	// connect the client socket to server socket
	if (connect(sockfd, (const struct sockadrr *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf(" There's an ERROR connecting to the server \n");
		exit(0);
	}
	else
	{
		printf(" Connection to the server is successfully done \n");
	}

	// function for chat
	mySend(sockfd);

	//Change CC from cubic to reno
	char buf[SIZE];
	strcpy(buf, "reno");
	socklen_t len = strlen(buf);
	if (setsockopt(sockfd, IPPROTO_IP, TCP_CONGESTION, buf, len) != 0)
	{
		perror("setsockopt");
		return -1;
	}
	len = strlen(buf);
	if (getsockopt(sockfd, IPPROTO_IP, TCP_CONGESTION, buf, &len) != 0)
	{
		perror("getsockopt");
		return -1;
	}
	mySend(sockfd);

	// close the socket
	close(sockfd);
	return 0;
}

// Reference:
// https://idiotdeveloper.com/file-transfer-using-tcp-socket-in-c/
// https://www.geeksforgeeks.org/socket-programming-cc/
// https://www.bogotobogo.com/cplusplus/sockets_server_client.php
// https://stackoverflow.com/questions/59265004/how-to-change-tcp-congestion-control-algorithm-using-setsockopt-call-from-c
// This exercise's document
//"cubic or reno- which is better?"
// "in both settings, paced Reno performs as well as CUBIC 
// and Compound TCP. We find that, CUBIC performs better than paced Reno in some scenarios, which is primarily due to SACK processing
//  that favors loss patterns of CUBIC flows in its loss recovery phase" -https://ieeexplore.ieee.org/document/7439950