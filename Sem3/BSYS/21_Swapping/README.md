# 21 Beyond Physical Memory: Mechanisms

[Chapter 21](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf)

~~~
FIELD DESCRIPTION FOR VM MODE
   Procs
       r: The number of runnable processes (running or waiting for run time).
       b: The number of processes in uninterruptible sleep.

   Memory
       swpd: the amount of virtual memory used.
       free: the amount of idle memory.
       buff: the amount of memory used as buffers.
       cache: the amount of memory used as cache.

   Swap
       si: Amount of memory swapped in from disk (/s).
       so: Amount of memory swapped to disk (/s).

   IO
       bi: Blocks received from a block device (blocks/s).
       bo: Blocks sent to a block device (blocks/s).

   System
       in: The number of interrupts per second, including the clock.
       cs: The number of context switches per second.

   CPU
       These are percentages of total CPU time.
       us: Time spent running non-kernel code.  (user time, including nice time)
       sy: Time spent running kernel code.  (system time)
       id: Time spent idle.  Prior to Linux 2.5.41, this includes IO-wait time.
       wa: Time spent waiting for IO.  Prior to Linux 2.5.41, included in idle.
       st: Time stolen from a virtual machine.  Prior to Linux 2.6.11, unknown.
~~~

## 1



~~~
running 1 instance:
	CPU user time is going up, even more, when multiple processes are running
~~~

## 2

~~~
when running ./mem 1024 the free space goes from 1119712 to 890456. There is no impact on the swpd.
when running ./mem 2048 the spwd is multiplied by about 4 and free nearly goes to 0. When killing the program the values change instantly.
The values are also represented in the swap columns. About the same size of memory are swapped in and out from memory.
~~~

## 3

~~~
cat /proc/meminfo
MemTotal:        2097152 kB
MemFree:         1977304 kB
MemAvailable:    1994764 kB
Buffers:               0 kB
Cached:            17460 kB
SwapCached:            0 kB
Active:            18052 kB
Inactive:          19980 kB
Active(anon):      11852 kB
Inactive(anon):    12268 kB
Active(file):       6200 kB
Inactive(file):     7712 kB
Unevictable:          16 kB
Mlocked:          191316 kB
SwapTotal:       5242880 kB
SwapFree:        5183892 kB
Dirty:               856 kB
Writeback:             0 kB
AnonPages:       7721928 kB
Mapped:          1670156 kB
Shmem:              1584 kB
KReclaimable:    6186808 kB
Slab:               0 kB
SReclaimable:          0 kB
SUnreclaim:            0 kB
KernelStack:       41840 kB
PageTables:       100692 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:    49861328 kB
Committed_AS:   30874360 kB
VmallocTotal:   34359738367 kB
VmallocUsed:           0 kB
VmallocChunk:          0 kB
Percpu:            73536 kB
HardwareCorrupted:     0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
CmaTotal:              0 kB
CmaFree:               0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
Hugetlb:               0 kB
DirectMap4k:     1627136 kB
DirectMap2M:    28454912 kB
DirectMap1G:     5242880 kB
~~~

When going over the memory the numbers never give non-zero values. And after the 1. loop the times are incredibly low. 

~~~
./mem 1986
allocating 2082471936 bytes (1986.00 MB)
  number of integers in array: 520617984
loop 0 in 11282.02 ms (bandwidth: 176.03 MB/s)
loop 1 in 134134.30 ms (bandwidth: 14.81 MB/s)
~~~

That makes sense since there are other processes running at the same time, using some memory. Therefore the OS has to swap out memory to the disk back and forth.
In the meantime the system can do other tasks to keep it running, but it still runs slow. That's because there is a continuous swap-in-out of pages in which the array is stored.
And because every circa 131000 ((2^31)/(2^12))/(2^2) iterations later a new page is touched.

## 4

~~~
I/O numer raise with ./mem 1986
CPU utilization stays the same
~~~

## 5
~~~
./mem 1024
allocating 1073741824 bytes (1024.00 MB)
  number of integers in array: 268435456
loop 0 in 5305.22 ms (bandwidth: 193.02 MB/s)
loop 1 in 1521.42 ms (bandwidth: 673.06 MB/s)
loop 2 in 1503.62 ms (bandwidth: 681.02 MB/s)
loop 3 in 1691.74 ms (bandwidth: 605.29 MB/s)
loop 4 in 1483.37 ms (bandwidth: 690.32 MB/s)
loop 5 in 1410.57 ms (bandwidth: 725.95 MB/s)
loop 6 in 1518.82 ms (bandwidth: 674.21 MB/s)
loop 7 in 1464.91 ms (bandwidth: 699.02 MB/s)
loop 8 in 1434.23 ms (bandwidth: 713.97 MB/s)
loop 9 in 1409.02 ms (bandwidth: 726.74 MB/s)
loop 10 in 1565.88 ms (bandwidth: 653.94 MB/s)
loop 11 in 1403.39 ms (bandwidth: 729.66 MB/s)
~~~

~~~
./mem 2400
allocating 2516582400 bytes (2400.00 MB)
  number of integers in array: 629145600
loop 0 in 35807.07 ms (bandwidth: 67.03 MB/s)
loop 1 in 152402.68 ms (bandwidth: 15.75 MB/s)
~~~
## 6

~~~
free
               total        used        free      shared  buff/cache   available
Mem:        2097152       99200     1961808         924       36144     1997952
Swap:       5242880       64464     5178416
~~~
