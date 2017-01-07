/******************************************
 * CS457 TCP Programming
 * client.c
 * Purpose: Send files over a network
 * @author Jacob Pankey
********************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 256

/********************************************
 * Function writes data recieved from server
 * to a file named output.txt. Continues to
 * read from socket until server reaches 
 * end of file
 * @param *arg - specifies the socket to read
 * data in from
 *******************************************/
void writeFile(void *arg)
{
	int serverSocket = *(int*)arg;
	char recvBuffer[BUF_SIZE];
	int bytesRecv = recv(serverSocket, recvBuffer,BUF_SIZE, 0);
	FILE * output = fopen("output.txt", "wb");
	if(!output)
	{
		perror("File error\n");
		return;
	}
	// When buffer is no longer full then this is
	// the last piece to send
	while(bytesRecv == BUF_SIZE )
	{
		fwrite(recvBuffer, bytesRecv, 1, output);
		bytesRecv = recv(serverSocket, recvBuffer, BUF_SIZE, 0);
	}
	// Write the last remaining piece
	fwrite(recvBuffer, bytesRecv, 1, output);
	fclose(output);
}


int main(int argc, char** arg)
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0)
	{
		printf("There was an error creating the socket\n");
		return 1;
	}

	int port;
	char ip[5000];
	printf("Which port?\n");
	scanf("%d", &port);
	printf("Which IP?\n");
	scanf("%s", &ip);

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	
	int e = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if (e < 0 )
	{
		printf("Error connecting\n");
		return 1;
	}
	char line[5000];

	// Client continuously loops to accept a new file
	// after the first finishes writing
	while(1)
	{
		printf("Which File would you like to recieve?\n");
		scanf("%s", &line);	
		send(sockfd,line,strlen(line),0);
		if(strcmp(line,"exit") == 0)
			break;
		writeFile(&sockfd);
		printf("Finished writing the file\n");
	}
	close(sockfd);
	return 0;
}
