# Optimizations

- realloc instead of free+malloc is a bug-ridden piece of shit. For some reason, reallocing both lower & upper made the last address of lower the same address as upper. That overwrote the partition obvs and caused a double free. WTF.

- made anything with number of threads unsigned char, N unsigned int for less memory and faster arithmetics

- made every const and static for compiler opt  

- memcpy to move memory blocks fast

# Evaluations

- compare pivot strategies by showing number of n per process each iteration

- according to grof and opfer, most time is spent in serial_qs when locally sorting the subarray

```
-------------------------------------------------------------------------------
355190   61.6654  quicksort                serial_qs
  355190   100.000  quicksort                serial_qs [self]
-------------------------------------------------------------------------------
```

- both iterative qs and qs + bubblesort were slower than regular qs

- qs is probably better than merge sort on average as it has better cache utilization

- binary search performs better than linear search for pivot element index