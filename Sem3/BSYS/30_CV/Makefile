
BINARIES = main-two-cvs-while main-two-cvs-if main-one-cv-while main-two-cvs-while-extra-unlock
HEADERS = mythreads.h main-header.h main-common.c pc-header.h

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

main-one-cv-while: main-one-cv-while.c $(HEADERS)
	gcc -o main-one-cv-while main-one-cv-while.c -pthread

main-two-cvs-if: main-two-cvs-if.c $(HEADERS)
	gcc -o main-two-cvs-if main-two-cvs-if.c -pthread

main-two-cvs-while: main-two-cvs-while.c $(HEADERS)
	gcc -o main-two-cvs-while main-two-cvs-while.c -pthread

main-two-cvs-while-extra-unlock: main-two-cvs-while-extra-unlock.c $(HEADERS)
	gcc -o main-two-cvs-while-extra-unlock main-two-cvs-while-extra-unlock.c -pthread


