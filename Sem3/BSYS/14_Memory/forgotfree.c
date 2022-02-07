#include <stdio.h>
#include <stdlib.h>

int main(void) {
    
    char *string = malloc(11);
    string = "Hallihallo";
    printf("%s\n", string);
    return 0;
}
