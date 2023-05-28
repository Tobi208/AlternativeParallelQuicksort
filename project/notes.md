# Optimizations

- realloc instead of free+malloc is a bug-ridden piece of shit. For some reason, reallocing both lower & upper made the last address of lower the same address as upper. That overwrote the partition obvs and caused a double free. WTF.

# Evaluations

- compare pivot strategies by showing number of n per process each iteration
