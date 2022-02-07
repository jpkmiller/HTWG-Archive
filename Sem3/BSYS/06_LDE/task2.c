#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}


int main(int argc, char *argv[]) {
	
	//pipe setup
    int PtoC[2]; //pipe parent to child
	int CtoP[2]; //pipe child to parent
	int child; //pid of child
	int i = 0;
	pipe(PtoC);
	pipe(CtoP);
    char buf = 'j'; //buffer

	//use only core 0
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	sched_setaffinity(getpid(), sizeof(set), &set);

	//time measuring
	struct timespec beginC;
	struct timespec endC;

	//precision
	struct timespec start;
	struct timespec stop;
	clock_gettime(CLOCK_REALTIME, &start);
	clock_gettime(CLOCK_REALTIME, &stop);
	long precision = stop.tv_nsec - start.tv_nsec;

    printf("%ld\n", precision);
	int it = atoi(argv[argc - 1]) * precision;
    unsigned long times[it];

	if ((child = fork()) < 0) {
        //fork failed
		perror("fork");
		exit(1);

	} else if (child == 0) {
        
        while (1) {
            read(PtoC[0], &buf, 1);
            write(CtoP[1], &buf, 1);        
        }

	} else {
        unsigned long mesD; //end - begin in nanosec

        while (i < it) {
            clock_gettime(CLOCK_REALTIME, &beginC);
            write(PtoC[1], &buf, 1); //write in parent to child pipe      		
            read(CtoP[0], &buf, 1); //context switch to child process
          	clock_gettime(CLOCK_REALTIME, &endC);
            
            mesD = ((((endC.tv_sec - beginC.tv_sec) * 1000000000) + endC.tv_nsec - beginC.tv_nsec) / 2); //2 context switches
            times[i++] = mesD;
            printf("%lu\r", mesD);
		}


        printf("\r"); //cr

        //time measurement
        unsigned long long mes = 0;

        //sorting array
        qsort (times, sizeof(times)/sizeof(*times), sizeof(*times), comp);
    
        //counting all times together -> leaving out the smallest and highest time
        printf("Sorting:\n");
        wait(1);   
        for(size_t j = 1; j < (unsigned long) i - 1; j++) {
            printf("%lu\r", times[j]);
      		mes += times[j];
      	}
        
        //printing final time and killing child
        printf("\rTime for context switch: %llu\n", mes / (i - 2));
        kill(child, SIGKILL);
	}
	return 0;
}
