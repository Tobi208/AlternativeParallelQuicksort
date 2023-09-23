# Alternative Parallel Quicksort

This was a university project for high performance and parallel computing with the challenge of implementing and optimizing an alternative parallel quicksort algorithm in C. While OpenMPI would have trivialized the implementation, the only allowed choices where Pthreads or OpenMP and unfortunately OpenMP does not offer the required low-level control over individual threads, so it had to be Pthreads.

Instead of adding more threads in each iteration/recursion (with e.g. OpenMP Tasks) as one would do for a trivial parallelisation of quicksort, this algorithm focuses on splitting the data up, sorting the chunks locally, and then merging them back up. It is more closely related to a *Merge-Splitting-Sort* as presented by *Selim G. Akl* in *Parallel Sorting Algorithms* but please check the report for details.

The merge process requires low-level synchronization of the threads but fine-graining the barriers allows for processes to pick up other threads when their current thread is waiting. Furthermore, the serial operations of the program have been operations in terms of arithmetics, memory, caching, and vectorization.

## Generate Test Data

```
$ make generator
$ mkdir -p INPUT_DIR
$ ./generator N INPUT_DIR DIST SEED
```

* N = input size
* INPUT_DIR = directory to store file
* DIST = distribution
	0. Randomly ordered integers from 0 to N -1
	1. Random integers
	2. Sorted integers from 0 to N - 1
	3. Sorted integers from N - 1 to 0
* SEED = random seed, 0: current time

or

```
$ make generate N=.. INPUT_DIR=.. DIST=.. SEED=..
```


## Compile

```
gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -lpthread
```

optional flags:

* -DCHECK_SORTED: checks if the output array is sorted
* -DCHECK_COMPLETE: checks if output[i] == i (do not use for Random Int Distribution)

or

```
$ make (-DCHECK_SORTED) (-DCHECK_COMPLETE)
```


## Run

```
$ ./quicksort N F T S
```

* N = Input size
* F = Binary file containing N integers
* T = Number of threads
* S = pivot strategy
	1. select the median of the local array of the first thread in each group
	2. select the mean value of all local medians in each group
	3. select the mean value of the center two medians in each group

or

```
$ make run N=.. F=.. T=.. S=..
```
