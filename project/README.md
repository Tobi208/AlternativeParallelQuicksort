# Generate Test Data

Generate an array from 1 to N and shuffle it using Fisher-Yates

```
$ make generate N=xxxxx INPUT_DIR=.....
```

```
$ make generator
$ mkdir -p INPUT_DIR
$ ./generator N INPUT_DIR
```

# Compile

```
$ make
```

```
gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -fopenmp
```

# Run

```
$ make run N=xxxxx INPUT_DIR=..... T=xx
```

```
$ ./quicksort N INPUT_FILE T
```
