#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
	int fd[2];
	pipe(fd);

	int rc = fork();

	if (rc < 0) {
		perror("failed creating child");
		exit(1);
	} else if (rc == 0) {
		printf("created child\n");
		char buffer[20];
		read(fd[0], &buffer, 20);
		printf("%s\n", buffer);
	} else {
		printf("created parent\n");
		write(fd[1], "Hello", 6);
		printf("written in pipe\n");
	}

	return 0;
}
