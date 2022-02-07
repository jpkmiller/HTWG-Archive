#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

volatile int counter = 0;

int main(void) {

	int rc = fork();
	if (rc < 0) {
		perror("fork failed");
		exit(1);
	} else if (rc == 0) {
		printf("I am a child with PID: %d\n", getpid());
		counter++;
		exit(0);
	} else {
		waitpid(-1, NULL, 0);
		printf("I am a parent with PID: %d and child %d\n", getpid(), rc);
		counter++;
		printf("counter: %d\n", counter);
	}
	return EXIT_SUCCESS;
}
