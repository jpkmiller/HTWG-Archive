#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	int retval = fork();
	
	printf("this is executing from %d\n", retval);

	if (retval == 0) {
		printf("Child am first\n");
	} else if (retval > 0) {
		wait(NULL);
		printf("I was waiting for child\n");
	} else {
		perror("fork failed");
	}
	
	printf("%d finishing\n", retval);
	return EXIT_SUCCESS;

}
