# 9 Lottery Scheduling

## 1

_Compute the solutions for simulations with 3 jobs and randm seeds of 1, 2, and 3._
~~~
python .\lottery-1.py -j 3 -s 1
ARG jlist
ARG jobs 3
ARG maxlen 10
ARG maxticket 100
ARG quantum 1
ARG seed 1

Here is the job list, with the run time of each job:
  Job 0 ( length = 1, tickets = 84 )
  Job 1 ( length = 7, tickets = 25 )
  Job 2 ( length = 4, tickets = 44 )
~~~

~~~
Here is the set of random numbers you will need (at most):
Random 651593 % 152 = 121 -> 2
Random 788724 % 152 = 148 -> 2
Random 93859 % 152 = 75 -> 0
JOB 0 is done
Random 28347 % 69 = 57 -> 2
Random 835765 % 69 = 37 -> 2
JOB 2 is done
THE REST IS JOB 1 AND THE RESULTS ARE NUMBERS <25
Random 432767 % 25 = 17
Random 762280
Random 2106
Random 445387
Random 721540
Random 228762
Random 945271
~~~

~~~
python .\lottery-1.py -j 3 -s 2
ARG jlist
ARG jobs 3
ARG maxlen 10
ARG maxticket 100
ARG quantum 1
ARG seed 2

Here is the job list, with the run time of each job:
  Job 0 ( length = 9, tickets = 94 )
  Job 1 ( length = 8, tickets = 73 )
  Job 2 ( length = 6, tickets = 30 )
~~~

~~~
Here is the set of random numbers you will need (at most):
Random 605944 % 197 = 169 -> 2
Random 606802 % 197 = 42 -> 0
Random 581204 % 197 = 54 -> 0
Random 158383 % 197 = 192 -> 2 
Random 430670 % 197 = 28 -> 0
Random 393532 % 197 = 123 -> 1
Random 723012 % 197 = 22 -> 0
Random 994820 % 197 = 167 -> 2
Random 949396 % 197 = 53 -> 0
Random 544177 % 197 = 63 -> 0
Random 444854 % 197 = 63 -> 0
Random 268241 % 197 = 28 -> 0
Random 35924 % 197 = 124 -> 1
Random 27444 % 197 = 70 -> 0
Random 464894 % 197 = 61 -> 0
JOB 0 is done
Random 318465 -> 1
Random 380015 -> 2
Random 891790 -> 1
Random 525753 -> 1
Random 560510 -> 2
Random 236123 -> 1
Random 23858 -> 1
JOB 1 is done
Random 325143 -> 2
~~~

~~~
python .\lottery-1.py -j 3 -s 3
ARG jlist
ARG jobs 3
ARG maxlen 10
ARG maxticket 100
ARG quantum 1
ARG seed 3

Here is the job list, with the run time of each job:
  Job 0 ( length = 2, tickets = 54 )
  Job 1 ( length = 3, tickets = 60 )
  Job 2 ( length = 6, tickets = 6 )
~~~

~~~
Here is the set of random numbers you will need (at most):
Random 13168 % 120 = 88 -> 1
Random 837469 % 120 = 109 -> 1
Random 259354 % 120 = 34 -> 0
Random 234331 % 120 = 91 -> 0
JOB 0 is done
Random 995645 % 66 = 35 -> 1
JOB 1 is done
THE REST IS JOB 2 AND THE RESULTS ARE NUMBERS <6
Random 470263 % 6 = 1 -> 2
Random 836462
Random 476353
Random 639068
Random 150616
Random 634861
~~~

## 2

_Now run with two specific jobs: each of length 10, but one (job 0) with just 1 ticket and the other (job 1) with 100 (e.g., -l 10:1,10:100). What happens when the number of tickets is so imbalanced? Will
job 0 ever run before job 1 completes? How often? In general, what does such a ticket imbalance do to the behavior of lottery scheduling?_

~~~
JOB 1 will run almost finish all its jobs before JOB 0 had run yet, only in a few cases
It's going to be very unfair for jobs with only a few tickets
~~~

## 3

_When running with two jobs of length 100 and equal ticket alloca- tions of 100 (-l 100:100,100:100), how unfair is the scheduler? Run with some different random seeds to determine the (probabilis- tic) answer; let unfairness be determined by how much earlier one job finishes than the other._

~~~
192/200 = 0,96
196/200 = 0,98
190/200 = 0,95
196/200 = 0,98
199/200 = 0,995
181/200 = 0,905

=> 0,961
~~~

## 4

_How does your answer to the previous question change as the quantum size (-q) gets larger?_

~~~
-q 10
150/200 = 0,75
160/200 = 0,8
190/200 = 0,95

-q 20
140/200 = 0,7
180/200 = 0,9
180/200 = 0,9

-q 50
150/200 = 0,75
150/200 = 0,75
100/200 = 0,5
~~~
