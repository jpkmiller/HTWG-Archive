#include <sys/mman.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
this is an example of how to use mmap.
best use case is when you have a simple txt file that you want to read.
you open it and mmap into the region (your virtual address space lol)

you can print it onto stdout (1)
*/

size_t get_filesize(const char *filename) {
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

int
main (int argc, char **argv) {
	assert(argc > 1); //should be given a file
	size_t filesize = get_filesize(argv[1]);
	int fd = open(argv[1], O_RDONLY, 0);
	assert(fd != -1);


	char * region = mmap(NULL, filesize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_POPULATE, fd, 0);
	assert(region != MAP_FAILED);
	printf("contents of region:\n%s", region);
	printf("EOF\n");
	printf("size of this region %lu\n", filesize);
	printf("position of region %p\n", region);
	printf("position of main %p\n", &main);

	assert(munmap(region, filesize) == 0);
	close(fd);
	return 0;
}
