#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
void func(void)
{
	int minFD = 0, maxFD = MAX;
	struct timespec t;
	unsigned long time;
	while(1) {
		fd_set readFDs;
		FD_ZERO(&readFDs);

		for (int fd = minFD; fd < maxFD; fd++) {
			FD_SET(fd, &readFDs);
		}

		int rc = select(maxFD + 1, &readFDs, NULL, NULL, NULL);

		for (int fd = minFD; fd < maxFD; fd++) {
			if (FD_ISSET(fd, &readFDs)) {
			}
				//process
		}

		/*
		   clock_gettime(CLOCK_REALTIME, &t);
		   time = t.tv_sec;
		   write(sockfd, &time, sizeof(time));

		   read(sockfd, &time, sizeof(time));
		   printf("\rfrom client : %lu\tto server", time); */
	}
}

int main(void)
{
	int mastersock, connfd, len, client_socket[MAX];
	struct sockaddr_in servaddr, cli;


	//init clients
	for (int i = 0; i < MAX; i++) {
		client_socket[i] = 0;
	}


	if (mastersock = socket(AF_INET, SOCK_STREAM, 0) == -1) {
		printf("master socket creation failed...\n");
		exit(1);
	}
	else
		printf("master socket successfully created..\n");

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("listen failed...\n");
		exit(0);
	}
	else
		printf("server listening..\n");
	len = sizeof(cli);


	FD_ZERO(&readFDs);
	FD_SET(master_socket, &readFDs);

	// Function for chatting between client and server
	func();

	// After chatting close the socket
	close(sockfd);
}
