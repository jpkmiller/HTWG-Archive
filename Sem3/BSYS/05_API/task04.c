#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
	int rc = fork();


	if (rc < 0) {
		printf("fork failed\n");
		exit(1);
	} else if (rc == 0) {
		printf("i am a child\n");
		/*
		char *execarg[2];
		execarg[0] = strdup("/bin/ls");
		execarg[1] = NULL;
		execvp(execarg[0], execarg);
		*/

		char *execarg2[5];
		execarg2[0] = strdup("/bin/ls");
		execarg2[1] = strdup("ls");
		execarg2[2] = strdup("-l");
		execarg2[3] = strdup("/");
		execarg2[4] = NULL;

		//execl(execarg2[0], execarg2[1], execarg2[2], NULL);
		//execlp(execarg2[0], execarg2[1], execarg2[2], NULL);
		//execv(execarg2[0], execarg2)
		//execve(execarg2[0], execarg2, NULL);



		} else {
		wait(NULL);
		printf("i am parent\n");
	}

	return 0;
}
