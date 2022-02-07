#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {

	int arg = atoi(argv[argc - 1]);
	struct timeval begin, end;
	struct timespec beginC, endC, resC;
	int usec, csec;

	if (strcmp(argv[1], "gettime") == 0) {

		for (int i = 0; i < arg; i++) {
			gettimeofday(&begin, (struct timezone *) 0); //gettimeofday -> writes in struct
			getpid();
			gettimeofday(&end, (struct timezone *) 0);
			if (strcmp(argv[2],"-l") == 0)
				printf("test-no. %d: %ld\n", i, end.tv_usec - begin.tv_usec); usec += end.tv_usec - begin.tv_usec;
		}

		printf("average time %d in usec\n", usec / arg); //time measurement with gettimeofday()

	} else {

		//clock_getres(CLOCK_REALTIME, &resC); printf("resolution is: %ld\n\n", resC.tv_nsec);
		for (int i = 0; i < arg; i++) { 
			clock_gettime(CLOCK_REALTIME, &beginC); 
			getpid(); 
			clock_gettime(CLOCK_REALTIME, &endC); 
			if (strcmp(argv[1], "-l") == 0)
				printf("test-no. %d: %d\n", i, (((int) endC.tv_sec - (int) beginC.tv_sec) * 1000000000) + (int) endC.tv_nsec - (int) 
			beginC.tv_nsec); csec += (int) ((endC.tv_sec - beginC.tv_sec) * 1000000000) + endC.tv_nsec - beginC.tv_nsec;
		}
		printf("average time %d in nsec\n", csec / arg); //time measurement with clock_gettime()

	}

}
