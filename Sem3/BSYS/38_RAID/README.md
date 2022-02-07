# 38 RAID

~~~
Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -D NUMDISKS, --numDisks=NUMDISKS
                        number of disks in RAID
  -C CHUNKSIZE, --chunkSize=CHUNKSIZE
                        chunk size of the RAID
  -n NUMREQUESTS, --numRequests=NUMREQUESTS
                        number of requests to simulate
  -S SIZE, --reqSize=SIZE
                        size of requests
  -W WORKLOAD, --workload=WORKLOAD
                        either "rand" or "seq" workloads
  -w WRITEFRAC, --writeFrac=WRITEFRAC
                        write fraction (100->all writes, 0->all reads)
  -R RANGE, --randRange=RANGE
                        range of requests (when using "rand" workload)
  -L LEVEL, --level=LEVEL
                        RAID level (0, 1, 4, 5)
  -5 RAID5TYPE, --raid5=RAID5TYPE
                        RAID-5 left-symmetric "LS" or left-asym "LA"
  -r, --reverse         instead of showing logical ops, show physical
  -t, --timing          use timing mode, instead of mapping mode
  -c, --compute         compute answers for me
~~~


## 1

Use the simulator to perform some basic RAID mapping tests. Runwith different levels (0, 1, 4, 5) and see if you can figure out the mappings of a set of requests. For RAID-5, see if you can figure out the difference between left-symmetric and left-asymmetric layouts. Use some different random seeds to generate different problems than above.

### RAID 0

~~~
disk   = address % number_of_disks (because the addresses are evenly distributed among n disks -> every n disks address restarts)

offset = address / number_of_disks (each disk has the same amount of chunks -> evenly divided)
~~~

python ./raid.py -L 0 -n 5 -s 2

~~~
ARG blockSize 4096
ARG seed 2
ARG numDisks 4
ARG chunkSize 4k
ARG numRequests 5
ARG reqSize 4k
ARG workload rand
ARG writeFrac 0
ARG randRange 10000
ARG level 0
ARG raid5 LS
ARG reverse False
ARG timing False

LOGICAL READ from addr:9560 size:4096
  disk = 9560 % 4 = 0
  offset = 9560 / 4 = 2390

LOGICAL READ from addr:565 size:4096
  disk = 565 % 4 = 1
  offset = 565 / 4 = 141

LOGICAL READ from addr:8354 size:4096
  disk = 8354 % 4 = 2
  offset = 8354 / 4 = 2088

LOGICAL READ from addr:6697 size:4096
  disk = 1
  offset = 1674

LOGICAL READ from addr:6059 size:4096
  disk = 3
  offset = 1514
~~~

### RAID 1

_for even-numbered logical blocks, the RAID chooses the even-numbered disk in the pair; the odd disk is used for odd-numbered logical blocks. This is done to make the results of each run easy to guess for you (instead of, for example, a random choice)_

~~~
It's not that difficult: you first calculate your offset and then your disk.
Even result & mod = 0 -> 0
Odd result & mod = 0 -> 1
Even result & mod = 1 -> 2
Odd result & mod = 1 -> 3
~~~

python ./raid.py -L 1 -n 5 -s 2

~~~
LOGICAL READ from addr:9560 size:4096
  disk = 9560 % 2 = 0
  offset = 9560 / 2 = 4780

LOGICAL READ from addr:565 size:4096
  disk = 565 % 2 = 2
  offset = 565 / 2 = 282

LOGICAL READ from addr:8354 size:4096
  disk = 8354 % 2 = 1
  offset = 8354 / 2 = 4177

LOGICAL READ from addr:6697 size:4096
  disk = 6697 % 2 = 1
  offset = 6697 / 2 = 3339

LOGICAL READ from addr:6059 size:4096
  disk = 6059 % 2 = 3
  offset = 6059 / 2 = 3029
~~~

### RAID 4

~~~
Same calculations as with RAID 0 -> use %3 and /3 because the 4th disk is for parity
~~~

python ./raid.py -L 4 -n 5 -s 2

~~~
LOGICAL READ from addr:9560 size:4096
  disk = 9560 % 3 = 2
  offset = 9560 / 3 = 3186

LOGICAL READ from addr:565 size:4096
  disk = 565 % 3 = 1
  offset = 565 / 3 = 188

LOGICAL READ from addr:8354 size:4096
  disk = 8354 % 3 = 2
  offset = 8354 / 3 = 2784

LOGICAL READ from addr:6697 size:4096
  disk = 6697 % 3 = 1
  offset = 6697 / 3 = 2232
  
LOGICAL READ from addr:6059 size:4096
  disk = 6059 % 3 = 2
  offset = 6059 / 3 = 2019

~~~

### RAID 5

~~~
D0  D1  D2  D3
0   1   2   3
4   5   6   7
8   9   10  11

with parity blocks (every logsical block moves one down)

LS:

D0  D1  D2  D3
0   1   2   P0
4   5   P1  3
8   P2   6  7
P3  9   10  11

LA:

D0  D1  D2  D3
0   1   2   P0
3   4   P1  5
6   P2   7  8
P3  9   10  11
~~~

## 2

Do the same as the first problem, but this time vary the chunk size with -C. How does chunk size change the mappings?

bigger chunks lead to new mapping in all RAIDS

### RAID 0

~~~
D0  D1  D2  D3
0   2   4   6
1   3   5   7
8   10  12  14
9   11  13  15
~~~

### RAID 5
~~~
LA:

D0  D1  D2  D3
0   2   4   P0
1   3   5   P1
6   8   P2  10
7   9   P3  11
~~~


## 3

Do the same as above, but use the -r flag to reverse the nature of each problem.

~~~
python ./raid.py -L 0 -n 12 -s 0 -W seq -C 8k -r
~~~

## 4

Now use the reverse flag but increase the size of each request wit the -S flag. Try specifying sizes of 8k, 12k, and 16k, while varying the RAID level. What happens to the underlying I/O pattern when the size of the request increases? Make sure to try this with the sequential workload too (-W sequential); for what request sizes are RAID-4 and RAID-5 much more I/O efficient?

### RAID 0

python raid.py -L 0 -n 5 -W seq -S 4k -r -s 1

~~~
LOGICAL OPERATION is 0
  read  [disk 0, offset 0]

LOGICAL OPERATION is 1
  read  [disk 1, offset 0]

LOGICAL OPERATION is 2
  read  [disk 2, offset 0]

LOGICAL OPERATION is 3
  read  [disk 3, offset 0]

LOGICAL OPERATION is 4
  read  [disk 0, offset 1]
~~~


python raid.py -L 0 -n 5 -W seq -S 8k -r -s 1

~~~
LOGICAL OPERATION is addr: 0, size:8192
  read  [disk 0, offset 0]
  read  [disk 1, offset 0]

LOGICAL OPERATION is addr: 2
  read  [disk 2, offset 0]
  read  [disk 3, offset 0]

LOGICAL OPERATION is addr: 4
  read  [disk 0, offset 1]
  read  [disk 1, offset 1]

LOGICAL OPERATION is addr: 6
  read  [disk 2, offset 1]
  read  [disk 3, offset 1]

LOGICAL OPERATION is addr: 8
  read  [disk 0, offset 2]
  read  [disk 1, offset 2]
~~~

## 5

Use the timing mode of the simulator (-t) to estimate the performance of 100 random reads to the RAID, while varying the RAID levels, using 4 disks.

Estimate how long the workload should take to complete.
- Roughly how many requests should each disk receive?
- How many requests are random, how many sequential?

### RAID 0

~~~

~~~

## 6

Do the same as above, but increase the number of disks. How does the performance of each RAID level scale as the number of disks increases?

## 7

Do the same as above, but use all writes (-w 100) instead of reads. How does the performance of each RAID level scale now? Can you do a rough estimate of the time it will take to complete the workload of 100 random writes?

## 8

Run the timing mode one last time, but this time with a sequential workload (-W sequential). How does the performance vary with RAID level, and when doing reads versus writes? How about when varying the size of each request? What size should you write to a RAID when using RAID-4 or RAID-5?
