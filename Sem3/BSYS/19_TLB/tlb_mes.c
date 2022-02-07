#define _CLOCK_ CLOCK_MONOTONIC_RAW
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Please restart with %s num-of-pages num-of-trials\n", argv[0]);
		exit(1);
	}

	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(2, &set);
	sched_setaffinity(getpid(), sizeof(set), &set);

	int PAGESIZE = getpagesize();
	int NUMPAGES = atoi(argv[1]);
	int NUMTRIALS = atoi(argv[2]);

	struct timespec begin;
	struct timespec end;

	long unsigned precision;

	int jump = PAGESIZE / sizeof(int); //1024
	int *a = calloc(NUMPAGES * jump, sizeof(int)); //num of pages * 1024
	long long unsigned diff;

	//precision

	clock_gettime(_CLOCK_, &begin);
	for (int j = 0; j < NUMTRIALS; j++)
	{
		for (int i = 0; i < NUMPAGES * jump; i += jump)
		{
		}
	}
	clock_gettime(_CLOCK_, &end);
	precision = ((end.tv_sec - begin.tv_sec) * 1000000000 + end.tv_nsec - begin.tv_nsec);

	//measurement


	clock_gettime(_CLOCK_, &begin);
	for (int j = 0; j < NUMTRIALS; j++)
	{
		for (int i = 0; i < NUMPAGES * jump; i += jump)
		{
			a[i] += 1;
		}
	}
	clock_gettime(_CLOCK_, &end);

	diff = (((end.tv_sec -begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec) - precision) / NUMPAGES )/ NUMTRIALS;
	//printf("num-p: %d -> time: %llu (prec: %lu, pagesize %d)\n", NUMPAGES, diff, precision / NUMPAGES / NUMTRIALS, PAGESIZE);
	printf("%d %llu\n", NUMPAGES, diff);
	free(a);
	return 0;
}

