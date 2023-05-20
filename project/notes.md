# Optimizations

- realloc instead of free+malloc is a bug-ridden piece of shit. For some reason, reallocing both lower & upper made the last address of lower the same address as upper. That overwrote the partition obvs and caused a double free. WTF.

# Evaluations

- compare pivot strategies by showing number of n per process each iteration

# Timings

unoptimized:
vitsippa
N = 100000000
1,13.716130
2,7.483095
4,3.999344
8,2.333471
16,1.620532
32,1.569978