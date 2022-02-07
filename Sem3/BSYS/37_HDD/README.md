# 37 HDD

_First, the rotational speed is by default set to 1 degree per time unit. Thus, to make a complete revolution, it takes 360 time units_

_Thus, to read a sector, it takes 30 time units (given our default speed of rotation_

_A seek from the outer track to the middle track takes 40 time units_

_You'd also have to know the scheduling policy. The default is FIFO, though, so for now you can just compute the request times assuming the processing order matches the list specified via the "-a" flag._

## 1

Compute the seek, rotation, and transfer times for the following sets of requests: -a 0, -a 6, -a 30, -a 7,30,8, and finally -a 10,11,12,13.


~~~
You have to calculate n - 1 rotations that have to be done, then add a quarter of a rotation which is 15 units and then add another 30 for the read
6 -> 0 = 150 + 15 + 30
6 -> 6 = 330 + 15 + 30
6 -> 30 = [330 (rotation from 30 to 30 ) - 80 (seek)] + 15 + 30 + 80 seek
6 -> 7,30,8 = [15 + 30] + [300 (bc 10 rotations going from 7 to 30) + 30] + [390 (bc 13 rotations going from 30 to 8) + 30]
6 -> 10,11,12,13 = [90 + 15 + 30] + [30] + [360 + 30] + [30]
~~~

## 2

Do the same requests above, but change the seek rate to different values: 
-S 2, -S 4, -S 8, -S 10, -S 40, -S 0.1. How do the times change?

~~~
When there is a seek like 11 -> 12 even with seek 40 it has to do a full rotation. When there is a seek like 11 -> 13 with seek set to 2 no full revolution is needed anymore ;)


python ./disk.py -a 7,30,8 -S 2 -c -G has the same results as with -S 1 because a seek of total 40 units is not sufficent to switch from inner to outer track in the same revolution.

with -S 4 the seek time is for two seeks 10 units and can therefore satisfy the request
~~~

## 3

Do the same requests above, but change the rotation rate: -R 0.1, -R 0.5, -R 0.01. How do the times change?

~~~
Everything is really slow. Transfers take longer 1/R longer e.g. a transfer that took 30 with -R 1 takes about 300 with -R 0.1
~~~

## 4

FIFO is not always best, e.g., with the request stream -a 7,30,8, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (-p SSTF) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?

~~~
6 -> 7,30,8 = [45] + [30] + [300 (bc. 10 rotations)]
Order changes depending on the shortest seek time seek(8) < seek(30) -> therefore 8 first


~~~


## 5

Now use the shortest access-time first (SATF) scheduler (-p SATF). Does it
make any difference for -a 7,30,8 workload? Find a set of requests where
SATF outperforms SSTF; more generally, when is SATF better than SSTF?

~~~
Same results as with -p SATF (Shortest Access Time First) which also orders the queue depending on seek time

./disk.py -a 31,6 -c -p SATF -S 40 -R 1
./disk.py -a 31,6 -c -p SSTF -S 40 -R 1

SATF shorter than SSTF when seek time is shorter than rotation time
~~~

## 6

Here is a request stream to try: -a 10,11,12,13. What goes poorly when it runs? Try adding track skew to address this problem (-o skew). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., -S 2, -S 4)? In general, could you write a formula to figure out the skew?

40 / 1 / (30 * 1) = 2

_skew = track-distance(40) / seek-speed / (rotational-space-degrees(360 / 12) * rotation-speed)_

~~~
-S 2: 40 / 2 / 30 ≈ 1
-S 4: 40 / 4 / 30 ≈ 1
~~~

~~~
In order not to skew too many tracks one could use a different scheduler like SATF ;)
~~~


## 7

Specify a disk with different density per zone, e.g., -z 10,20,30, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., -a -1 -A 5,-1,0, which specifies that random requests should be used via the -a -1 flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?

~~~
middle: 2/(339 + 260)       =
outer:  3/(135 + 270 + 140) =
~~~


## 8

A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., -A 1000,-1,0, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the -c flag and don’t turn on graphics (-G) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?

~~~
python ./disk.py -a -1 -A 1000,-1,0 -p SATF -w 1 -c
python ./disk.py -a -1 -A 1000,-1,0 -p SATF -w 10 -c
python ./disk.py -a -1 -A 1000,-1,0 -p SATF -w 100 -c
python ./disk.py -a -1 -A 1000,-1,0 -p SATF -w 200 -c

It is logarithmic => the bigger the scheduling window is the better the scheduler performs till it reaches a minimum of ca. 35000 time units

FIFO is not affected by the window because it takes the request one by one and handles it without changing the order
~~~

## 9

Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., -w 4); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?

~~~
python ./disk.py -a 12,7,8,9,10,11 -p SATF -c          // 7,8,9,10,11,12 Total: 555
python ./disk.py -a 12,7,8,9,10,11 -p BSATF -w 4 -c    // 7,8,9,12,10,11 Total: 525
~~~

## 10

All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?

~~~
python ./disk.py -a 9,20 -c            // 435
python ./disk.py -a 9,20 -c -p SATF    // 465
~~~
