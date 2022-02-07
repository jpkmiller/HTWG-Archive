## 1

_How does the output change from run to run?_

~~~
./vector-deadlock -n 2 -l 1 -v
->add(0, 1)
<-add(0, 1)
              ->add(0, 1)
              <-add(0, 1)


./vector-deadlock -n 2 -l 1 -v
              ->add(0, 1)
              <-add(0, 1)
->add(0, 1)
<-add(0, 1)

~~~
Orders differ in which the threads run


## 2

_Now add the -d flag, and change the number of loops (-l) from 1 to higher numbers. What happens? Does the code (always) deadlock?_

The code deadlocks quite often when executing without verbose

~~~
./vector-deadlock -n 2 -l 100 -d

./vector-deadlock -n 2 -l 1000 -d
~~~

## 3

_How does changing the number of threads (-n) change the outcome of the program? Are there any values of -n that ensure no deadlock occurs?_

when having about 50 threads => -n 50
the program is deadlocking with already 10 loops => -l 10

only when changing to -n 0 or -n 1 there will be no deadlocks anymore

## 4

_Now examine the code in vector-global-order.c. do you understand why the code avoids deadlock?_

It looks at the order in memory and locks always the lock coming first in memory first.

_Also, why is there a special case in this vector add() routine when the source and destination vectors are the same?_

when same locks are given. Mostly never happens only if the same lock is given twice

## 5

_Now run the code with the following flags: -t -n 2 -l 100000 -d. How long does the code take to complete?_ 

about 0.11 seconds

_How does the total time change when you increase the number of loops, or the number of threads?_

time increases

## 6

_What happens if you turn on the parallelism flag (-p)? How much would you expect performance to change when each thread is working on adding different vectors (which is what -p enables) versus working on the same ones?_

I would expect that the code would run much faster. n threads are given n different locks which means that they don't interfere which each other

After running: it takes about half the time

## 7

_Now let’s study vector-try-wait.c. First make sure you understand the code. Is the first call to pthread mutex trylock() really needed?_

~~~c
top:
    if (pthread_mutex_trylock(&v_dst->lock) != 0) {
        goto top;
    }

    if (pthread_mutex_trylock(&v_src->lock) != 0) {
        retry++;
        Pthread_mutex_unlock(&v_dst->lock);
        goto top;
    }

~~~

when the dst lock could not be acquired it returns to the top.
when the src lock could not be acquired it releases the dst lock,
updates a retry counter and returns to the top

_Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?_

~~~
./vector-global-order -t -n 2 -l 10000000
Time: 13.13 seconds

./vector-try-wait -n 2 -l 10000000 -t
Retries: 78169
Time: 6.66 seconds
~~~


vector-try-wait is faster than the global order

## 8

_Now let’s look at vector-avoid-hold-and-wait.c. What is the main problem with this approach?_ 

only one lock at a time is allowed. Therefore a parallelism doesn't work either (-p)

_How does its performance compare to the other versions, when running both with -p and without it?_

it will be worse than others because it can't perform in parallel

## 9

_Finally, let’s look at vector-nolock.c. This version doesn’t use locks at all; does it provide the exact same semantics as the other versions? Why or why not?_

it does not because there is no lock at all. Instead it uses a atomic instruction (all in one) to update the vector

~~~c
asm volatile("lock; xaddl %%eax, %2;"
                 :"=a" (value)
                 :"a" (value), "m" (*variable)
                 :"memory");

...

for (i = 0; i < VECTOR_SIZE; i++) {
    fetch_and_add(&v_dst->values[i], v_src->values[i]);
}
~~~

## 10

_Now compare its performance to the other versions, both when threads are working on the same two vectors (no -p) and when each thread is working on separate vectors (-p). How does this no-lock version perform?_

it takes longer than the other methods
