#include <stdio.h>

int main(void) {

	int *x = 0;
	int y = *x + 1;
	printf("%d\n", y);
	return 0;
}
