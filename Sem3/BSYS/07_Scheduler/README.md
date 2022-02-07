# Scheduling: Introduction
[Chapter 7](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched.pdf)

## 1

~~~
python '.\scheduler 1.py' -p FIFO -j 3 -s 200
ARG policy FIFO
ARG jobs 3
ARG maxlen 10
ARG seed 200

Here is the job list, with the run time of each job:
  Job 0 ( length = 1 )
  Job 1 ( length = 3 )
  Job 2 ( length = 8 )
~~~

>Compute the turnaround time, response time, and wait time for each job.
>When you are done, run this program again, with the same arguments,
>but with -c, which will thus provide you with the answers. You can use
>-s <somenumber> or your own job list (-l 10,15,20 for example)
>to generate different problems for yourself.

### Answer (FIFO & SJF)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 1 | 4 | 12 |
| Response-Time |0 | 1 | 4 |

### Solution (FIFO & SJF)

~~~
Execution trace:
  [ time   0 ] Run job 0 for 1.00 secs ( DONE at 1.00 )
  [ time   1 ] Run job 1 for 3.00 secs ( DONE at 4.00 )
  [ time   4 ] Run job 2 for 8.00 secs ( DONE at 12.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 1.00  Wait 0.00
  Job   1 -- Response: 1.00  Turnaround 4.00  Wait 1.00
  Job   2 -- Response: 4.00  Turnaround 12.00  Wait 4.00

  Average -- Response: 1.67  Turnaround 5.67  Wait 1.67
~~~

## 2
#### 2.1

~~~
python '.\scheduler 1.py' -p FIFO -j 3 -s 100
ARG policy FIFO
ARG jobs 3
ARG maxlen 10
ARG seed 100

Here is the job list, with the run time of each job:
  Job 0 ( length = 2 )
  Job 1 ( length = 5 )
  Job 2 ( length = 8 )
~~~

### Answer (FIFO & SJF)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 2 | 7 | 15 |
| Response-Time |0 | 2 | 7 |

### Solution (FIFO & SJF)

~~~
Execution trace:
  [ time   0 ] Run job 0 for 2.00 secs ( DONE at 2.00 )
  [ time   2 ] Run job 1 for 5.00 secs ( DONE at 7.00 )
  [ time   7 ] Run job 2 for 8.00 secs ( DONE at 15.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 2.00  Wait 0.00
  Job   1 -- Response: 2.00  Turnaround 7.00  Wait 2.00
  Job   2 -- Response: 7.00  Turnaround 15.00  Wait 7.00

  Average -- Response: 3.00  Turnaround 8.00  Wait 3.00
~~~

#### 2.2

~~~
python '.\scheduler 1.py' -p FIFO -j 3 -s 300
ARG policy FIFO
ARG jobs 3
ARG maxlen 10
ARG seed 300

Here is the job list, with the run time of each job:
  Job 0 ( length = 6 )
  Job 1 ( length = 4 )
  Job 2 ( length = 1 )
~~~

### Answer (FIFO)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 6 | 10 | 11 |
| Response-Time |6 | 6 | 10 |


### Solution (FIFO)

~~~
Execution trace:
  [ time   0 ] Run job 0 for 6.00 secs ( DONE at 6.00 )
  [ time   6 ] Run job 1 for 4.00 secs ( DONE at 10.00 )
  [ time  10 ] Run job 2 for 1.00 secs ( DONE at 11.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 6.00  Wait 0.00
  Job   1 -- Response: 6.00  Turnaround 10.00  Wait 6.00
  Job   2 -- Response: 10.00  Turnaround 11.00  Wait 10.00

  Average -- Response: 5.33  Turnaround 9.00  Wait 5.33
~~~

### Answer (SJF)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 1 | 5 | 11 |
| Response-Time | 0 | 1 | 5 |

### Solution (SJF)

~~~
Execution trace:
  [ time   0 ] Run job 2 for 1.00 secs ( DONE at 1.00 )
  [ time   1 ] Run job 1 for 4.00 secs ( DONE at 5.00 )
  [ time   5 ] Run job 0 for 6.00 secs ( DONE at 11.00 )

Final statistics:
  Job   2 -- Response: 0.00  Turnaround 1.00  Wait 0.00
  Job   1 -- Response: 1.00  Turnaround 5.00  Wait 1.00
  Job   0 -- Response: 5.00  Turnaround 11.00  Wait 5.00

  Average -- Response: 2.00  Turnaround 5.67  Wait 2.00
~~~
## 3
#### 3.1
~~~
python '.\scheduler 1.py' -p RR-q 1 -j 3 -s 100
ARG policy RR-q 1 -j 3 -s 100
ARG jobs 3
ARG maxlen 10
ARG seed 100

Here is the job list, with the run time of each job:
  Job 0 ( length = 2 )
  Job 1 ( length = 5 )
  Job 2 ( length = 8 )
~~~

### Answer (RR)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 4 | 11 | 15 |
| Response-Time | 0 | 1 | 2 |

### Solution (RR)

~~~
Execution trace:
  [ time   0 ] Run job   0 for 1.00 secs
  [ time   1 ] Run job   1 for 1.00 secs
  [ time   2 ] Run job   2 for 1.00 secs
  [ time   3 ] Run job   0 for 1.00 secs ( DONE at 4.00 )
  [ time   4 ] Run job   1 for 1.00 secs
  [ time   5 ] Run job   2 for 1.00 secs
  [ time   6 ] Run job   1 for 1.00 secs
  [ time   7 ] Run job   2 for 1.00 secs
  [ time   8 ] Run job   1 for 1.00 secs
  [ time   9 ] Run job   2 for 1.00 secs
  [ time  10 ] Run job   1 for 1.00 secs ( DONE at 11.00 )
  [ time  11 ] Run job   2 for 1.00 secs
  [ time  12 ] Run job   2 for 1.00 secs
  [ time  13 ] Run job   2 for 1.00 secs
  [ time  14 ] Run job   2 for 1.00 secs ( DONE at 15.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 4.00  Wait 2.00
  Job   1 -- Response: 1.00  Turnaround 11.00  Wait 6.00
  Job   2 -- Response: 2.00  Turnaround 15.00  Wait 7.00

  Average -- Response: 1.00  Turnaround 10.00  Wait 5.00
~~~

#### 3.2

~~~
python '.\scheduler 1.py' -p RR -q 1 -j 3 -s 200
ARG policy RR
ARG jobs 3
ARG maxlen 10
ARG seed 200

Here is the job list, with the run time of each job:
  Job 0 ( length = 1 )
  Job 1 ( length = 3 )
  Job 2 ( length = 8 )
~~~

### Answer (RR)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 1 | 6 | 12 |
| Response-Time | 0 | 1 | 2 |

### Solution (RR)

~~~
Execution trace:
  [ time   0 ] Run job   0 for 1.00 secs ( DONE at 1.00 )
  [ time   1 ] Run job   1 for 1.00 secs
  [ time   2 ] Run job   2 for 1.00 secs
  [ time   3 ] Run job   1 for 1.00 secs
  [ time   4 ] Run job   2 for 1.00 secs
  [ time   5 ] Run job   1 for 1.00 secs ( DONE at 6.00 )
  [ time   6 ] Run job   2 for 1.00 secs
  [ time   7 ] Run job   2 for 1.00 secs
  [ time   8 ] Run job   2 for 1.00 secs
  [ time   9 ] Run job   2 for 1.00 secs
  [ time  10 ] Run job   2 for 1.00 secs
  [ time  11 ] Run job   2 for 1.00 secs ( DONE at 12.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 1.00  Wait 0.00
  Job   1 -- Response: 1.00  Turnaround 6.00  Wait 3.00
  Job   2 -- Response: 2.00  Turnaround 12.00  Wait 4.00

  Average -- Response: 1.00  Turnaround 6.33  Wait 2.33
~~~

#### 3.3

~~~
python '.\scheduler 1.py' -p RR -q 1 -j 3 -s 300                                                                                                    
ARG policy RR
ARG jobs 3
ARG maxlen 10
ARG seed 300

Here is the job list, with the run time of each job:
  Job 0 ( length = 6 )
  Job 1 ( length = 4 )
  Job 2 ( length = 1 )
~~~

### Answer (RR)

| | Job 0 | Job 1 | Job 2 |
| ------ | ------ | ------ | ------ |
| Turnaround-Time | 11 | 9 | 3 |
| Response-Time | 0 | 1 | 2 |

### Solution (RR)

~~~
Execution trace:
  [ time   0 ] Run job   0 for 1.00 secs
  [ time   1 ] Run job   1 for 1.00 secs
  [ time   2 ] Run job   2 for 1.00 secs ( DONE at 3.00 )
  [ time   3 ] Run job   0 for 1.00 secs
  [ time   4 ] Run job   1 for 1.00 secs
  [ time   5 ] Run job   0 for 1.00 secs
  [ time   6 ] Run job   1 for 1.00 secs
  [ time   7 ] Run job   0 for 1.00 secs
  [ time   8 ] Run job   1 for 1.00 secs ( DONE at 9.00 )
  [ time   9 ] Run job   0 for 1.00 secs
  [ time  10 ] Run job   0 for 1.00 secs ( DONE at 11.00 )

Final statistics:
  Job   0 -- Response: 0.00  Turnaround 11.00  Wait 5.00
  Job   1 -- Response: 1.00  Turnaround 9.00  Wait 5.00
  Job   2 -- Response: 2.00  Turnaround 3.00  Wait 2.00

  Average -- Response: 1.00  Turnaround 7.67  Wait 4.00
~~~

## 4

_For what types of workloads does SJF deliver the same turnaround times as FIFO?_

SJF has same turnaround times as FIFO, when the jobs are already sorted from the shortest to the longest job.
Furthermore they have to start at the same time (?)

## 5

_For what types of workloads and quantum lengths does SJF deliver the same response times as RR?_

SJF has the the same response times as RR when RR quantum is longer or equally long as the longest job AND the jobs have to be already in order from the shortest to the longest job
