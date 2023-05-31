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
static int sorted(const int* xs, const int N) {
    for (int i = 0; i < N - 1; i++)
        if (xs[i] > xs[i + 1])
            return 0;
    return 1;
}

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

        // recursion
        serial_qs(xs, lo, i - 1);
        serial_qs(xs, i + 1, hi);
    }
}

/**
 * Find split point of an array according to a pivot element
 * https://en.wikipedia.org/wiki/Binary_search_algorithm
*/
static int split(const int* xs, const int n, const int p) {
    int s, lo = 0, mi, hi = n - 1;
    while (lo < hi) {
        mi = lo + (hi - lo) / 2;
        if (p < xs[mi])
            hi = mi - 1;
        else
            lo = mi + 1;
    }
    for (s = hi - 1; xs[s + 1] <= p && s < n - 1; s++);
    return s;
}

/**
 * Merge two arrays ys and zs on xs
*/
static void merge(int* xs, const int* ys, const int* zs, const int ys_lo, const int ys_hi, const int zs_lo, const int zs_hi) {
    int i = 0, j = ys_lo, k = zs_lo;
    while (j <= ys_hi && k <= zs_hi)
        if (ys[j] < zs[k])  xs[i++] = ys[j++];
        else                xs[i++] = zs[k++];
    for (; j <= ys_hi; j++) xs[i++] = ys[j];
    for (; k <= zs_hi; k++) xs[i++] = zs[k];
}

/**
 * Input arguments for the thread workers,
 * only the thread id changes per thread
*/
typedef struct static_args_t {
    unsigned int N;
    unsigned char T;
    int **yss;
    int *xs, *r_lo, *r_hi, *ps, *ns, *ms;
    pthread_barrier_t *bs_local, *bs_group;
} static_args_t;

typedef struct args_t {
    unsigned char  tid;
    static_args_t* static_args;
} args_t;

static void* thread_worker(void* targs) {

    // copy input
    const args_t*        args   = (args_t*) targs;
    const unsigned char  tid    = args->tid;
    const static_args_t* s_args = args->static_args;
    const unsigned int   N      = s_args->N;
    const unsigned char  T      = s_args->T;
    int*  xs   = s_args->xs;
    int** yss  = s_args->yss;
    int*  r_lo = s_args->r_lo;
    int*  r_hi = s_args->r_hi;
    int*  ps   = s_args->ps;
    int*  ns   = s_args->ns;
    int*  ms   = s_args->ns;
    pthread_barrier_t* bs_local = s_args->bs_local;
    pthread_barrier_t* bs_group = s_args->bs_group;

    // inclusive lower bound of this thread's subarray on xs
    int lo = tid * (N / T);
    // inclusive upper bound of this thread's subarray on xs
    int hi = tid < T - 1 ? (tid + 1) * (N / T) - 1 : N - 1;
    // total number of elements in this thread's subarray on xs
    int n = hi - lo + 1;

    // copy to local subarray
    int* ys  = (int*) malloc(n * sizeof(int));
    yss[tid] = ys;
    memcpy(ys, xs + lo, n * sizeof(int));
    int* zs;

    // sort subarray locally
    serial_qs(ys, 0, n - 1);

    unsigned char lid, gid, rid; // local id, group id, partner thread id
    unsigned char t = T;         // threads per group
    int p, s;                    // value and index of pivot element
    int l_lo, l_hi;              // boundaries of subarray split according to p

    // for finding the correct barriers in the collective arrays
    unsigned char shift_bs_group = 0;
    unsigned char shift_bs_local = 0;
    unsigned char num_gs         = 1;

    // divide threads into groups until no smaller groups can be formed
    while (t > 1) {

        lid = tid % t;
        gid = tid / t;

        // current median
        ms[tid] = ys[n / 2];

        // wait for threads to finish splitting in previous iteration
        // otherwise they might split by the pivot element of the next iteration
        // also their median might be outdated
        pthread_barrier_wait(bs_group + shift_bs_group + gid);
        if (lid == 0) {

            // strategy 1
            // median of thread 0 of each group
            p = ms[tid - lid];

            // strategy 2
            // mean of all medians in a group
            // unsigned long int mm = 0;
            // for (unsigned char i = tid; i < tid + t; i++)
            //     mm += ms[i];
            // p = mm / t;

            // strategy 3
            // mean of center 2 medians in a group
            // serial_qs(ms, tid, tid + t - 1);
            // p = (ms[tid + t / 2] + ms[tid + t / 2] + 1) / 2;

            // distribute pivot element within group
            for (unsigned char i = tid; i < tid + t; i++)
                    ps[i] = p;
        }
        pthread_barrier_wait(bs_group + shift_bs_group + gid);

        // pivot element
        p = ps[tid];
        s = split(ys, n, p);

        // send data
        if (lid < t / 2) {
            // send upper part, receive lower part
            rid = tid + t / 2;
            l_lo = 0; 
            l_hi = s;
            r_lo[tid] = s + 1;
            r_hi[tid] = n - 1;
            shift_bs_local = rid;
        } else {
            // send lower part, receive upper part
            rid = tid - t / 2;
            l_lo = s + 1;
            l_hi = n - 1;
            r_lo[tid] = 0;
            r_hi[tid] = s;
            shift_bs_local = tid;
        }
        // use barrier of the upper partner
        pthread_barrier_wait(bs_local + shift_bs_local);

        // merge local and remote elements in place of new local subarray
        n = l_hi - l_lo + r_hi[rid] - r_lo[rid] + 2;
        zs = (int*) malloc(n * sizeof(int));
        merge(zs, ys, yss[rid], l_lo, l_hi, r_lo[rid], r_hi[rid]);
        pthread_barrier_wait(bs_local + shift_bs_local);

        // iterate
        free(ys);
        ys = zs;
        yss[tid] = ys;
        t = t >> 1;
        shift_bs_group += num_gs;
        num_gs = num_gs << 1;
    }

    // merge local subarrays back into xs
    // reservate space on xs
    ns[tid] = n;
    if (T > 1) // else bs_group was never allocated
        pthread_barrier_wait(bs_group);
    // find location of local subarray on xs
    int n_prev = 0;
    for (unsigned char i = 0; i < tid; i++)
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
void parallel_qs(int* xs, const unsigned int N, const unsigned char T) {

    int** yss = (int**) malloc(T * sizeof(int*)); // local subarrays
    int* r_lo = (int*)  malloc(T * sizeof(int )); // lo bound of exchanged subarray
    int* r_hi = (int*)  malloc(T * sizeof(int )); // hi bound of exchanged subarray
    int* ps   = (int*)  malloc(T * sizeof(int )); // pivot element for each thread
    int* ns   = (int*)  malloc(T * sizeof(int )); // number of elements in each thread in xs
    int* ms   = (int*)  malloc(T * sizeof(int )); // median of each subarray

    pthread_barrier_t* bs_local = (pthread_barrier_t*) malloc(T * sizeof(pthread_barrier_t));
    for (unsigned char i = 0; i < T; i++)
        pthread_barrier_init(bs_local + i, NULL, 2);

    // let T = 16, then
    // bs_group = [ 0, 0, 1, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7]
    // b_counts = [16, 8, 8, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2]
    // num_bs_g =  1  +  2  +     4     +          8
    int num_bs_group = 0;
    for (unsigned char j = 1; j < T; j = j << 1)
        num_bs_group += j;
    pthread_barrier_t* bs_group = (pthread_barrier_t*) malloc(num_bs_group * sizeof(pthread_barrier_t));
    unsigned char l = 0;
    for (unsigned char j = 1; j < T; j = j << 1)
        for (unsigned char k = 0; k < j; k++)
            pthread_barrier_init(bs_group + l++, NULL, T / j);

    static_args_t static_args = {N, T, yss, xs, r_lo, r_hi, ps, ns, ms, bs_local, bs_group};
    pthread_t* threads = (pthread_t*) malloc(T * sizeof(pthread_t));

    // fork
    for (unsigned char i = 0; i < T; i++) {
        // malloc because otherwise it will reuse pointers or something
        args_t* targs = (args_t*) malloc(sizeof(args_t));
        targs->tid = i;
        targs->static_args = &static_args;
        pthread_create(threads + i, NULL, thread_worker, (void*) targs);
    }

    // join
    for (unsigned char i = 0; i < T; i++)
        pthread_join(threads[i], NULL);

    // free shared memory
    free(yss);
    free(r_lo);
    free(r_hi);
    free(ps);
    free(ns);
    free(threads);
    for (unsigned char i = 0; i < T; i++)
        pthread_barrier_destroy(bs_local + i);
    for (unsigned char i = 0; i < num_bs_group; i++)
        pthread_barrier_destroy(bs_group + i);
    free(bs_local);
    free(bs_group);
}

int main(int argc, char** argv) {

    // parse input
    if (argc != 4) {
        printf("Usage: ./quicksort N (input size) file (binary file containing integers) T (number of threads such that T = 2^k and 0 < T < 256)\n");
        exit(-1);
    }
    const unsigned int N = (unsigned int) atoi(argv[1]);
    char* filename = argv[2];
    const unsigned char T = (unsigned char) atoi(argv[3]);
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
    if ((T & (T - 1)) != 0) {
        printf("T must be defined such that T = 2^k and 0 < T < 256\n");
        exit(-1);
    }

    // read data
    int* xs = read_file(filename, N);

    // parallel quicksort
    const double timer = get_wall_seconds();
    parallel_qs(xs, N, T);
    printf("%f\n", get_wall_seconds() - timer);

    // check correctness
    if (!sorted(xs, N)) {
        printf("Result array is not sorted!\n");
        exit(-1);
    }

    // free
    free(xs);
    return 0;
}