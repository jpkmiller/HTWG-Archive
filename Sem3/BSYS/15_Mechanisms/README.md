# 15 Translation Mechanisms

## 1

python .\relocation-1.py -s 1

~~~
ARG seed 1
ARG address space size 1k
ARG phys mem size 16k

Base-and-Bounds register information:

  Base   : 0x0000363c (decimal 13884)
  Limit  : 290

For each virtual address, either write down the physical address it translates to
OR write down that it is an out-of-bounds address (a segmentation violation). For
this problem, you should assume a simple virtual address space of a given size.
~~~

### Answer: (s 1)
~~~
Virtual Address Trace
  VA  0: 0x0000030e (decimal:  782) violation
  VA  1: 0x00000105 (decimal:  261) valid (13884 + 261 = 14145)
  VA  2: 0x000001fb (decimal:  507) violation
  VA  3: 0x000001cc (decimal:  460) violation
  VA  4: 0x0000029b (decimal:  667) violation
~~~

~~~
Base-and-Bounds register information:

  Base   : 0x00003ca9 (decimal 15529)
  Limit  : 500
~~~

### Answer: (s 2)

~~~
Virtual Address Trace
  VA  0: 0x00000039 (decimal:   57) valid (15586)
  VA  1: 0x00000056 (decimal:   86) valid (15615)
  VA  2: 0x00000357 (decimal:  855) violation
  VA  3: 0x000002f1 (decimal:  753) violation
  VA  4: 0x000002ad (decimal:  685) violation
~~~

~~~
Base-and-Bounds register information:

  Base   : 0x000022d4 (decimal 8916)
  Limit  : 316
~~~

### Answer:
~~~
Virtual Address Trace
  VA  0: 0x0000017a (decimal:  378) violation
  VA  1: 0x0000026a (decimal:  618) violation
  VA  2: 0x00000280 (decimal:  640) violation
  VA  3: 0x00000043 (decimal:   67) valid (8983)
  VA  4: 0x0000000d (decimal:   13) valid (8929)
~~~

## 2

Value of -l has to be 930

## 3

16284
