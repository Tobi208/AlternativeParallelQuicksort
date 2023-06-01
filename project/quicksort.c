#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

/**
 * Get the wall time in seconds
 * as taken from the lecture
*/
static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

/**
 * Read list of integers from file
 * as taken from the lecture
*/
static int* read_file(char* filename, const int N) {
    
    int* xs = (int*) malloc(N * sizeof(int));
    FILE* file = fopen(filename, "r");
    int m = fread(xs, sizeof(int), N, file);
    fclose(file);

    if (m != N) {
        printf("Unable to read file: %s\n", filename);
        exit(-1);
    }

    return xs;
}

/**
 * Swap two integer values
 * as taken from the lecture
*/
static void swap(int* x, int* y) {
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

/**
 * Check if an array is sorted
*/
#ifdef CHECK_SORTED
static int sorted(const int* xs, const unsigned int N) {
    for (unsigned int i = 0; i < N - 1; i++)
        if (xs[i] > xs[i + 1])
            return 0;
    return 1;
}
#endif

/**
 * Check if the array is complete
 * i.e. contains 0, 1, 2, .., N - 1 in order
*/
#ifdef CHECK_COMPLETE
static int complete(const int* xs, const unsigned int N) {
    for (unsigned int i = 0; i < N - 1; i++)
        if (xs[i] != i)
            return 0;
    return 1;
}
#endif

/**
 * Use classic quicksort on a subsection of an array
 * https://en.wikipedia.org/wiki/Quicksort
*/
static void serial_qs(int* xs, const int lo, const int hi) {
    if (lo < hi) {
        // lomuto partition scheme
        int i = lo - 1;
        for (int j = lo; j < hi; j++)
            if (xs[j] <= xs[hi])
                swap(xs + ++i, xs + j);
        swap(xs + ++i, xs + hi);
        serial_qs(xs, lo, i - 1);
        serial_qs(xs, i + 1, hi);
    }
}

/**
 * Find split point of an array according to a pivot element
 * https://en.wikipedia.org/wiki/Binary_search_algorithm
*/
static int split(const int* xs, const unsigned int n, const int p) {
    int s, lo = 0, mi, hi = n - 1;
    while (lo < hi) {
        mi = lo + (hi - lo) / 2;
        if (p < xs[mi])
            hi = mi - 1;
        else
            lo = mi + 1;
    }
    // right shift split into place after unsuccessful search
    for (s = hi - 1; (s < n && xs[s] <= p) || s < 0; s++);
    return s;
}

/**
 * Merge two arrays ys and zs on xs
 * as taken from the lecture
*/
static void merge(int* xs, const int* ys, const int* zs, const unsigned int y_n, const unsigned int z_n) {
    int i = 0, j = 0, k = 0;
    while (j < y_n && k < z_n)
        if (ys[j] < zs[k])  xs[i++] = ys[j++];
        else                xs[i++] = zs[k++];
    for (; j < y_n; j++)    xs[i++] = ys[j];
    for (; k < z_n; k++)    xs[i++] = zs[k];
}

/**
 * Input arguments for the thread workers,
 * only the thread id changes per thread
*/
typedef struct static_args_t {
    unsigned int   N;
    unsigned short T;
    unsigned char  S;
    int **yss, **rss;
    int *xs, *rns, *ps, *ns, *ms;
    pthread_barrier_t *bs_local, *bs_group;
} static_args_t;

typedef struct args_t {
    unsigned short tid;
    static_args_t* static_args;
} args_t;

static void* thread_worker(void* targs) {

    // copy input
    const args_t*        args   = (args_t*) targs;
    const unsigned short tid    = args->tid;         // thread num
    const static_args_t* s_args = args->static_args;
    const unsigned int   N      = s_args->N;         // input size
    const unsigned short T      = s_args->T;         // num threads
    const unsigned char  S      = s_args->S;         // pivot strategy
    int*  xs   = s_args->xs;  // global input array
    int** yss  = s_args->yss; // local subarrays
    int** rss  = s_args->yss; // local subarrays shifted to pivot index
    int*  rns  = s_args->rns; // size from/to pivot index in local subarrays
    int*  ps   = s_args->ps;  // pivot elements per local subarray
    int*  ns   = s_args->ns;  // size of local subarrays
    int*  ms   = s_args->ns;  // median elements per local subarray
    pthread_barrier_t* bs_local = s_args->bs_local; // pairs barriers
    pthread_barrier_t* bs_group = s_args->bs_group; // group barriers

    // inclusive lower bound of this thread's subarray on xs
    unsigned int lo = tid * (N / T);
    // inclusive upper bound of this thread's subarray on xs
    unsigned int hi = tid < T - 1 ? (tid + 1) * (N / T) - 1 : N - 1;
    // total number of elements in this thread's subarray on xs
    unsigned int n = hi - lo + 1;

    // copy to local subarray otherwise reallocating won't work
    int* ys  = (int*) malloc(n * sizeof(int));
    yss[tid] = ys;
    memcpy(ys, xs + lo, n * sizeof(int));
    int* zs;
    ms[tid] = 0;

    // sort subarray locally
    serial_qs(ys, 0, n - 1);

    unsigned short lid, gid, rid; // local id, group id, partner thread id
    unsigned short t = T;         // threads per group
    int p;                        // value of pivot element
    unsigned int s;               // index of pivot element + 1
    unsigned int ln;              // number of elements of subarray split according to p
    unsigned int lshift;          // index shift of local subarray to reach split

    // for finding the correct barriers in the collective arrays
    unsigned short shift_bs_group = 0;
    unsigned short shift_bs_local = 0;
    unsigned short num_gs         = 1;

    // divide threads into groups until no smaller groups can be formed
    while (t > 1) {

        lid = tid % t;
        gid = tid / t;

        // update current median if elements exist
        // what is a good solution for n == 0?
        if (n > 0)
            ms[tid] = ys[n / 2];

        // wait for threads to finish splitting in previous iteration
        // otherwise they might split by the pivot element of the next iteration
        // also their median might not be updated yet
        pthread_barrier_wait(bs_group + shift_bs_group + gid);
        if (lid == 0) {
            if (S == 1) {
                // strategy 1
                // median of thread 0 of each group
                p = ms[tid];
            } else if (S == 2) {
                // strategy 2
                // mean of all medians in a group
                long int mm = 0;
                for (unsigned short i = tid; i < tid + t; i++)
                    mm += ms[i];
                p = mm / t;
            } else if (S == 3) {
                // strategy 3
                // mean of center 2 medians in a group
                serial_qs(ms, tid, tid + t - 1);
                p = (ms[tid + t / 2 - 1] + ms[tid + t / 2]) / 2;
            } else {
                // default to strategy 1
                p = ms[tid];
            }
            // distribute pivot element within group
            for (unsigned short i = tid; i < tid + t; i++)
                    ps[i] = p;
        }
        pthread_barrier_wait(bs_group + shift_bs_group + gid);

        // find split point according to pivot element
        p = ps[tid];
        s = split(ys, n, p);

        // send data
        if (lid < t / 2) {
            // send upper part
            rid      = tid + t / 2; // remote partner id
            lshift   = 0;           // local shift to fit split point
            ln       = s;           // local size of lower part
            rss[tid] = ys + s;      // remote shift to fit split point
            rns[tid] = n - s;       // remote size of parnter's lower part
            shift_bs_local = rid;   // shift to find partner's barrier
        } else {
            // send lower part
            rid      = tid - t / 2;
            lshift   = s;
            ln       = n - s;
            rss[tid] = ys;
            rns[tid] = s;
            shift_bs_local = tid;
        }
        // use barrier of the upper partner
        pthread_barrier_wait(bs_local + shift_bs_local);

        // merge local and remote elements in place of new local subarray
        n = ln + rns[rid];
        zs = (int*) malloc(n * sizeof(int));
        merge(zs, ys + lshift, rss[rid], ln, rns[rid]);
        pthread_barrier_wait(bs_local + shift_bs_local);

        // iterate
        free(ys);
        ys              = zs;          // swap local pointer
        yss[tid]        = ys;          // update global pointer
        t               = t >> 1;      // half group size
        shift_bs_group += num_gs;      // shift group barrier pointer
        num_gs          = num_gs << 1; // double number of groups
    }

    // merge local subarrays back into xs
    // reservate space on xs
    ns[tid] = n;
    if (T > 1) // else bs_group was never allocated
        pthread_barrier_wait(bs_group);
    // find location of local subarray on xs
    unsigned int n_prev = 0;
    for (unsigned short i = 0; i < tid; i++)
        n_prev += ns[i];
    // copy
    memcpy(xs + n_prev, ys, n * sizeof(int));

    // free thread local memory
    free(targs);
    free(ys);
    return NULL;
}

/**
 * Use parallel quicksort on an array
 * as interpreted from QuickSort.pdf
*/
void parallel_qs(int* xs, const unsigned int N, const unsigned short T, const unsigned char S) {

    int** yss = (int**) malloc(T * sizeof(int*)); // local subarrays
    int** rss = (int**) malloc(T * sizeof(int*)); // remote subarrays
    int*  rns = (int*)  malloc(T * sizeof(int )); // number of elements 'sent'
    int*  ps  = (int*)  malloc(T * sizeof(int )); // pivot element for each thread
    int*  ns  = (int*)  malloc(T * sizeof(int )); // number of elements in each thread in xs
    int*  ms  = (int*)  malloc(T * sizeof(int )); // median of each subarray

    pthread_barrier_t* bs_local = (pthread_barrier_t*) malloc(T * sizeof(pthread_barrier_t));
    for (unsigned short i = 0; i < T; i++)
        pthread_barrier_init(bs_local + i, NULL, 2);

    // let T = 16, then
    // bs_group = [ 0, 0, 1, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7]
    // b_counts = [16, 8, 8, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2]
    // num_bs_g =  1  +  2  +     4     +          8
    unsigned short num_bs_group = 0;
    for (unsigned short j = 1; j < T; j = j << 1)
        num_bs_group += j;
    pthread_barrier_t* bs_group = (pthread_barrier_t*) malloc(num_bs_group * sizeof(pthread_barrier_t));
    unsigned short l = 0;
    for (unsigned short j = 1; j < T; j = j << 1)
        for (unsigned short k = 0; k < j; k++)
            pthread_barrier_init(bs_group + l++, NULL, T / j);

    static_args_t static_args = {N, T, S, yss, rss, xs, rns, ps, ns, ms, bs_local, bs_group};
    pthread_t* threads = (pthread_t*) malloc(T * sizeof(pthread_t));

    // fork
    for (unsigned short i = 0; i < T; i++) {
        // malloc because otherwise it will reuse pointers or something
        args_t* targs = (args_t*) malloc(sizeof(args_t));
        targs->tid = i;
        targs->static_args = &static_args;
        pthread_create(threads + i, NULL, thread_worker, (void*) targs);
    }

    // join
    for (unsigned short i = 0; i < T; i++)
        pthread_join(threads[i], NULL);

    // free shared memory
    free(yss);
    free(rss);
    free(rns);
    free(ps);
    free(ns);
    free(ms);
    free(threads);
    for (unsigned short i = 0; i < T; i++)
        pthread_barrier_destroy(bs_local + i);
    for (unsigned short i = 0; i < num_bs_group; i++)
        pthread_barrier_destroy(bs_group + i);
    free(bs_local);
    free(bs_group);
}

int main(int argc, char** argv) {

    // parse input
    if (argc != 5) {
        printf("Usage: ./quicksort N F T S\n");
        printf("       N - input size\n");
        printf("       F - binary file containing integers\n");
        printf("       T - number of threads such that T = 2^k\n");
        printf("       S - strategy to select pivot element {1, 2, 3}\n");
        exit(-1);
    }
    const int N    = atoi(argv[1]);
    char* filename = argv[2];
    const unsigned short T = (unsigned short) atoi(argv[3]);
    const char S = (char) atoi(argv[3]);
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
    if ((T & (T - 1)) != 0) {
        printf("T must be defined such that T = 2^k\n");
        exit(-1);
    }

    // read data
    int* xs = read_file(filename, N);

    // parallel quicksort
    const double timer = get_wall_seconds();
    parallel_qs(xs, N, T, S);
    printf("%f\n", get_wall_seconds() - timer);

    // check correctness
#ifdef CHECK_SORTED
    if (!sorted(xs, N)) {
        printf("Result array is not sorted!\n");
        exit(-1);
    }
#endif
#ifdef CHECK_COMPLETE
    if (!complete(xs, N)) {
        printf("Result array is not complete!\n");
        exit(-1);
    }
#endif

    // free
    free(xs);
    return 0;
}