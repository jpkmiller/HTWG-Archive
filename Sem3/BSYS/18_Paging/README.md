# 18 Paging: Introduction

[Chapter 18](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf)

## 1
_Before doing any translations, let’s use the simulator to study how linear page tables change size given different parameters. Compute the size of linear page tables as different parameters change. Some suggested inputs are below; by using the -v flag, you can see how many page-table entries are filled. First, to understand how linear page table size changes as the address space grows, run with these flags:
-P 1k -a 1m -p 512m -v -n 0 -P 1k -a 2m -p 512m -v -n 0 -P 1k -a 4m -p 512m -v -n 0_

_Then, to understand how linear page table size changes as page size grows:
 -P 1k -P 2k -P 4k -a 1m -p 512m -v -n 0 -a 1m -p 512m -v -n 0 -a 1m -p 512m -v -n 0_

_Before running any of these, try to think about the expected trends. How should page-table size change as the address space grows? As the page size grows? Why not use big pages in general_
~~~
By increasing the addressspace with a constant page size the size of pagetable increases by factor 2^n
And by increasing the page size with a constant addressspace size the pagetable decreases by factor 2^n
When having really big pages the effiency of having a pagetable is getting lost. Because then you have a really big offset and have to alot of comparisons.
~~~

## 2
_Now let’s do some translations. Start with some small examples, and change the number of pages that are allocated to the address space with the -u flag. For example:
-P 1k -a 16k -p 32k -v -u 0 -P 1k -a 16k -p 32k -v -u 25 -P 1k -a 16k -p 32k -v -u 50 -P 1k -a 16k -p 32k -v -u 75 -P 1k -a 16k -p 32k -v -u 100
What happens as you increase the percentage of pages that are al- located in each address space?_
~~~
More and more PTE will get valid. Therefore all virtual addresses will be valid
~~~

## 3
_Now let’s try some different random seeds, and some different (and sometimes quite crazy) address-space parameters, for variety:
-P 8 -a 32 -p 1024 -v -s 1 
-P 8k -a32k -p 1m -v -s 2 
-P 1m -a 256m -p 512m -v -s 3
Which of these parameter combinations are unrealistic? Why?_
~~~
1. In a real computer system there is never such a small address space

2. The problem with these parameters is that the physical memory is 1m and the address space is only 32k. This means that a very small
fraction of the address space is mapped to a really big physical memory.

3. These parameters are completely unrealistic because the addressspace is only the half of the physical memory.
Furthermore the pages have a size of only 1m in a pagetable with the size of 256m. Therefore there can be 256 PTEs.
~~~
