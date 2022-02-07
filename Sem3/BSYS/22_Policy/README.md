# 22 Paging Policy

[Chapter 22](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys-policy.pdf)

## 1
~~~
python paging-policy.py -s 0 -n 10
ARG addresses -1
ARG addressfile
ARG numaddrs 10
ARG policy FIFO
ARG clockbits 2
ARG cachesize 3
ARG maxpage 10
ARG seed 0
ARG notrace False

Assuming a replacement policy of FIFO, and a cache of size 3 pages,
figure out whether each of the following page references hit or miss
in the page cache.
~~~


-s 0 FIFO
~~~
Access: 8  Miss FI 8		LI
Access: 7  Miss	FI 8 7		LI
Access: 4  Miss FI 8 7 4	LI
Access: 2  Miss FI 7 4 2	LI
Access: 5  Miss	FI 4 2 5	LI
Access: 4  Hit	FI 4 2 5	LI
Access: 7  Miss	FI 2 5 7	LI
Access: 3  Miss FI 5 7 3	LI
Access: 4  Miss	FI 7 3 4	LI
Access: 5  Miss FI 3 4 5	LI
~~~

-s 0 LRU
~~~
Access: 8  Miss LRU 8		MRU
Access: 7  Miss LRU 8 7 	MRU
Access: 4  Miss LRU 8 7 4	MRU
Access: 2  Miss LRU 7 4 2	MRU
Access: 5  Miss LRU 4 2 5	MRU
Access: 4  Hit	LRU 2 5 4	MRU
Access: 7  Miss	LRU 5 4 7 	MRU
Access: 3  Miss	LRU 4 7 3	MRU
Access: 4  Hit	LRU 7 3 4	MRU
Access: 5  Miss	LRU 3 4 5	MRU
~~~

-s 0 OPT
~~~
Access: 8  Miss	Left 8		Right
Access: 7  Miss Left 8 7	Right
Access: 4  Miss Left 8 7 4	Right
Access: 2  Miss Left 7 4 2	Right
Access: 5  Miss Left 7 4 5	Right
Access: 4  Hit	Left 7 4 5	Right
Access: 7  Hit	Left 7 4 5	Right
Access: 3  Miss	Left 4 5 3	Right
Access: 4  Hit	Left 4 5 3	Right
Access: 5  Hit 	Left 4 5 3	Right
~~~

-s 1 FIFO
~~~
Access: 1  Miss FI 1		LI
Access: 8  Miss FI 1 8		LI
Access: 7  Miss	FI 1 8 7	LI
Access: 2  Miss	FI 8 7 2	LI
Access: 4  Miss FI 7 2 4	LI
Access: 4  Hit	FI 7 2 4	LI
Access: 6  Miss	FI 2 4 6	LI
Access: 7  Miss	FI 4 6 7	LI
Access: 0  Miss	FI 6 7 0	LI
Access: 0  Hit	FI 6 7 0	LI
~~~

-s 1 LRU
~~~
Access: 1  Miss LRU 1 		MRU
Access: 8  Miss LRU 1 8		MRU
Access: 7  Miss LRU 1 8 7	MRU
Access: 2  Miss LRU 8 7 2	MRU
Access: 4  Miss LRU 7 2 4	MRU
Access: 4  Hit	LRU 7 2 4	MRU
Access: 6  Miss	LRU 2 4 6	MRU
Access: 7  Miss LRU 4 6 7	MRU
Access: 0  Miss LRU 6 7 0	MRU
Access: 0  Hit 	LRU 6 7 0	MRU
~~~

-s 1 OPT
~~~
Access: 1  Miss	Left 1		Right
Access: 8  Miss	Left 1 8	Right
Access: 7  Miss Left 1 8 7	Right
Access: 2  Miss	Left 1 7 2	Right
Access: 4  Miss	Left 1 7 4	Right
Access: 4  Hit	Left 1 7 4	Right
Access: 6  Miss	Left 1 7 6	Right
Access: 7  Hit	Left 1 7 6	Right
Access: 0  Miss	Left 1 7 0	Right
Access: 0  Miss Left 1 7 0	Right
~~~

-s 2 FIFO
~~~
Access: 9  Miss FI 9 		LI
Access: 9  Hit	FI 9 		LI
Access: 0  Miss	FI 9 0		LI
Access: 0  Hit	FI 9 0		LI
Access: 8  Miss	FI 9 0 8	LI
Access: 7  Miss	FI 0 8 7	LI
Access: 6  Miss FI 8 7 6	LI
Access: 3  Miss	FI 7 6 3	LI
Access: 6  Hit	FI 7 6 3	LI
Access: 6  Hit 	FI 7 6 3	LI
~~~

-s 2 LRU
~~~
Access: 9  Miss	LRU 9		MRU
Access: 9  Hit	LRU 9		MRU
Access: 0  Miss LRU 9 0		MRU
Access: 0  Hit	LRU 9 0		MRU
Access: 8  Miss	LRU 9 0 8	MRU
Access: 7  Miss	LRU 0 8 7	MRU
Access: 6  Miss	LRU 8 7 6	MRU
Access: 3  Miss	LRU 7 6 3	MRU
Access: 6  Hit  LRU 7 3 6	MRU
Access: 6  Hit	LRU 7 3 6	MRU
~~~

-s 2 OPT
~~~
Access: 9  Miss	Left 9		Right
Access: 9  Hit	Left 9		Right
Access: 0  Miss	Left 9 0	Right
Access: 0  Hit	Left 9 0	Right
Access: 8  Miss	Left 9 0 8	Right
Access: 7  Miss Left 9 0 7	Right
Access: 6  Miss Left 9 0 6	Right
Access: 3  Miss Left 9 6 3	Right
Access: 6  Hit	Left 9 6 3	Right
Access: 6  Hit	Left 9 6 3	Right
~~~

## 2

~~~
ARG addressfile
ARG numaddrs 10
ARG policy FIFO
ARG clockbits 2
ARG cachesize 5
ARG maxpage 10
ARG seed 0
ARG notrace False
~~~

~~~c
python paging-policy.py -p FIFO -C 5 -a 1,2,3,4,5
~~~

~~~
ARG addresses 1,2,3,4,5

Solving...

Access: 1  MISS FirstIn ->          [1] <- Lastin  Replaced:- [Hits:0 Misses:1]
Access: 2  MISS FirstIn ->       [1, 2] <- Lastin  Replaced:- [Hits:0 Misses:2]
Access: 3  MISS FirstIn ->    [1, 2, 3] <- Lastin  Replaced:- [Hits:0 Misses:3]
Access: 4  MISS FirstIn -> [1, 2, 3, 4] <- Lastin  Replaced:- [Hits:0 Misses:4]
Access: 5  MISS FirstIn -> [1, 2, 3, 4, 5] <- Lastin  Replaced:- [Hits:0 Misses:5]
~~~

~~~c
python paging-policy.py -p LRU -C 5 -a 1,2,3,4,5,6,1,2,3,4,5
~~~

~~~
ARG addresses 1,2,3,4,5,6,1,2,3,4,5

Access: 1  MISS LRU ->          [1] <- MRU Replaced:- [Hits:0 Misses:1]
Access: 2  MISS LRU ->       [1, 2] <- MRU Replaced:- [Hits:0 Misses:2]
Access: 3  MISS LRU ->    [1, 2, 3] <- MRU Replaced:- [Hits:0 Misses:3]
Access: 4  MISS LRU -> [1, 2, 3, 4] <- MRU Replaced:- [Hits:0 Misses:4]
Access: 5  MISS LRU -> [1, 2, 3, 4, 5] <- MRU Replaced:- [Hits:0 Misses:5]
Access: 6  MISS LRU -> [2, 3, 4, 5, 6] <- MRU Replaced:1 [Hits:0 Misses:6]
Access: 1  MISS LRU -> [3, 4, 5, 6, 1] <- MRU Replaced:2 [Hits:0 Misses:7]
Access: 2  MISS LRU -> [4, 5, 6, 1, 2] <- MRU Replaced:3 [Hits:0 Misses:8]
Access: 3  MISS LRU -> [5, 6, 1, 2, 3] <- MRU Replaced:4 [Hits:0 Misses:9]
Access: 4  MISS LRU -> [6, 1, 2, 3, 4] <- MRU Replaced:5 [Hits:0 Misses:10]
Access: 5  MISS LRU -> [1, 2, 3, 4, 5] <- MRU Replaced:6 [Hits:0 Misses:11]

FINALSTATS hits 0   misses 11   hitrate 0.00
~~~

~~~c
python paging-policy.py -p MRU -C 5 -a 1,2,3,4,5,6,7,8,9          
~~~

~~~
ARG addresses 1,2,3,4,5,6,7,8,9


Access: 1  MISS LRU ->          [1] <- MRU Replaced:- [Hits:0 Misses:1]
Access: 2  MISS LRU ->       [1, 2] <- MRU Replaced:- [Hits:0 Misses:2]
Access: 3  MISS LRU ->    [1, 2, 3] <- MRU Replaced:- [Hits:0 Misses:3]
Access: 4  MISS LRU -> [1, 2, 3, 4] <- MRU Replaced:- [Hits:0 Misses:4]
Access: 5  MISS LRU -> [1, 2, 3, 4, 5] <- MRU Replaced:- [Hits:0 Misses:5]
Access: 6  MISS LRU -> [1, 2, 3, 4, 6] <- MRU Replaced:5 [Hits:0 Misses:6]
Access: 7  MISS LRU -> [1, 2, 3, 4, 7] <- MRU Replaced:6 [Hits:0 Misses:7]
Access: 8  MISS LRU -> [1, 2, 3, 4, 8] <- MRU Replaced:7 [Hits:0 Misses:8]
Access: 9  MISS LRU -> [1, 2, 3, 4, 9] <- MRU Replaced:8 [Hits:0 Misses:9]

FINALSTATS hits 0   misses 9   hitrate 0.00
~~~

