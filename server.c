/***********************************
 * CS457 TCP Programming
 * server.c
 * Purpose: read file specified by client
 * and send data to the client
 * @author Jacob Pankey
************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 256
/*********************************************
 * This function opens the file specified by
 * fileName and reads the file 256 bytes at a 
 * time. These bytes are sent to socket arg to
 * be written to disk
 * @param fileName specifies the file to send
 * @param arg specifies the socket to send to
 ********************************************/
void sendFile(char *fileName, void *arg)
{
	FILE *fp = fopen(fileName, "rb");
	int clientSocket = *(int*)arg;
	char sendBuffer[BUF_SIZE];

	if(!(fp))
	{
		printf("ERROR: FILE %s NOT FOUND\n", fileName);
		return;
	}
	int bool = 1;
	int bytesRead;

	// Read file and send data chunks until end of file is reached
	while(bool)
	{
		bytesRead = fread(sendBuffer, 1 , sizeof(sendBuffer), fp);
		send(clientSocket, sendBuffer, bytesRead, 0);

		// True when last data chunk is sent. Set bool to 0
		// and exit the while loop
		if (bytesRead < BUF_SIZE)
		{
			if(feof(fp))
				printf("End of file\n");
			if(ferror(fp))
				printf("Error reading\n");
			bool = 0;				
		}
	}
}
/******************************************
 * This function waits for the client to choose
 * the file that it wants sent by the server.
 * Once the server gets a file name it calls
 * send file to transfer the data
 * @param arg specifies the socket to read the
 * file name from
*******************************************/
void getFileName(void *arg)
{
	char fileName[5000];
	int clientSocket = *(int*)arg;
	
	// This loops allows the client to continuously
	// request new files until the user types exit
	// to end the connection
	while(1)
	{
		// Clear previous file name
		memset(fileName, '\0', 5000);
		strcpy(fileName, "\0");
		printf("Waiting to recieve a file name\n");
		int i = recv(clientSocket, fileName, 5000, 0);

		if(strcmp(fileName,"exit") == 0)
		{
			printf("Client Disconnected \n");
			close(clientSocket);
			return;
		}
		else
		{
			sendFile(fileName, &clientSocket);
		}
	
	}
}


int main(int argc,char ** argv)
{
	/*Used for listening not communication*/
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	int port;
	printf("Which port would you like to listen to?\n");
	scanf("%d", &port);
		

	struct sockaddr_in serveraddr,clientaddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(port);
	serveraddr.sin_addr.s_addr=INADDR_ANY;
	
	bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	listen(sockfd,10);
	
	int len = sizeof(clientaddr);
	char *fileName;
	int clientSocket, *new_sock;

	// Each time a new client connnection is accepted
	// a new thread is created. This thread calls getFileName
	// The parent thread returns to wait for a new client to connect
	while(clientSocket = accept(sockfd, (struct sockaddr*) &clientaddr, &len))
	{
		pthread_t child;
		new_sock = malloc(sizeof(new_sock));
		*new_sock = clientSocket;
		pthread_create(&child, NULL, getFileName,(void*) new_sock);
	}

	close(clientSocket);
	
	
	return 0;
}

