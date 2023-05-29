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
        if (xs[i] > xs[i + 1]) {
            printf("ERROR - i = %d, xs[i] = %d, xs[i + 1] = %d\n", i, xs[i], xs[i + 1]);
            return 0;
        }
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
                swap(&xs[++i], &xs[j]);
        swap(&xs[++i], &xs[hi]);

        // recursion
        serial_qs(xs, lo, i - 1);
        serial_qs(xs, i + 1, hi);
    }
}

/**
 * Split array into lower and upper arrays according to pivot element
*/
static void split(int* xs, int* ys, int* zs, const int n, const int p) {
    int j = 0, k = 0;
    for (int i = 0; i < n; i++)
        if (xs[i] <= p) ys[j++] = xs[i];
        else            zs[k++] = xs[i];
}

/**
 * Merge two arrays ys and zs on xs
*/
static void merge(int* xs, int* ys, int* zs, const int size_y, const int size_z) {
    int i = 0, j = 0, k = 0;
    while (j < size_y && k < size_z)
        if (ys[j] < zs[k])  xs[i++] = ys[j++];
        else                xs[i++] = zs[k++];
    for (; j < size_y; j++) xs[i++] = ys[j];
    for (; k < size_z; k++) xs[i++] = zs[k];
}

/**
 * Input arguments for the thread workers,
 * only the thread id changes per thread
*/
typedef struct static_args_t {
    unsigned int N;
    unsigned char T;
    int **recv;
    int *xs, *recv_counts, *ps, *ns;
    pthread_barrier_t *bs_local, *bs_group;
} static_args_t;

typedef struct args_t {
    unsigned char tid;
    static_args_t* static_args;
} args_t;

static void* thread_worker(void* targs) {

    // copy input
    const args_t* args = (args_t*) targs;
    const unsigned char tid = args->tid;
    const static_args_t* s_args = args->static_args;
    const unsigned int N = s_args->N;
    const unsigned char T = s_args->T;
    int* xs = s_args->xs;
    int** recv = s_args->recv;
    int* recv_counts = s_args->recv_counts;
    int* ps = s_args->ps;
    int* ns = s_args->ns;
    pthread_barrier_t* bs_local = s_args->bs_local;
    pthread_barrier_t* bs_group = s_args->bs_group;

    // inclusive lower bound of this thread's subarray on xs
    int lo = tid * (N / T);
    // inclusive upper bound of this thread's subarray on xs
    int hi = tid < T - 1 ? (tid + 1) * (N / T) - 1 : N - 1;
    // total number of elements in this thread's subarray on xs
    int n = hi - lo + 1;

    // copy to local subarray
    int* ys = (int*) malloc(n * sizeof(int));
    memcpy(ys, xs + lo, n * sizeof(int));

    // sort subarray locally
    serial_qs(ys, 0, n - 1);

    unsigned char lid, gid; // local id, group id
    unsigned char t = T;    // threads per group
    int p;                  // value of pivot element

    // arrays for elements lower/higher than the pivot element
    int *lower, *upper;
    int lower_count, upper_count;

    // arrays from elements received from another process and for retained elements
    int *remote, *local;
    int remote_count, local_count;

    // for finding the correct barriers in the collective arrays
    unsigned char shift_bs_group = 0;
    unsigned char shift_bs_local = 0;
    unsigned char num_gs = 1;

    // divide threads into groups until no smaller groups can be formed
    while (t > 1) {

        lid = tid % t;
        gid = tid / t;

        // strategy 1
        // median of thread 0 of each group
        if (lid == 0)
            p = ys[n / 2];
        
        // wait for threads to finish splitting in previous iteration
        // otherwise they might split by the pivot element of the next iteration
        pthread_barrier_wait(bs_group + shift_bs_group + gid);

        // distribute pivot element within group
        if (lid == 0)
            for (unsigned char i = tid; i < tid + t; i++)
                    ps[i] = p;
        pthread_barrier_wait(bs_group + shift_bs_group + gid);

        // count number of elements lower/higher than pivot element
        p = ps[tid];
        lower_count = 0;
        for (int i = 0; i < n; i++)
            if (ys[i] <= p)
                lower_count++;
        upper_count = n - lower_count;

        // realloc is evil, do not use
        lower = (int*) malloc(lower_count * sizeof(int));
        upper = (int*) malloc(upper_count * sizeof(int));

        // split local subarray into lower/upper arrays
        split(ys, lower, upper, n, p);

        /* Data exchange pattern:
         * t = 8
         * lower groups   upper groups
         *    0 1 2 3   |   4 5 6 7
         * then
         * 0 <-> 4, 1 <-> 5, 2 <-> 6, 3 <-> 7
         */

        // send data
        if (lid < t / 2) {
            // send upper part, receive lower part
            recv[tid + t / 2] = upper;
            recv_counts[tid + t / 2] = upper_count;
            local = lower;
            local_count = lower_count;
            shift_bs_local = tid + t / 2;
        } else {
            // send lower part, receive upper part
            recv[tid - t / 2] = lower;
            recv_counts[tid - t / 2] = lower_count;
            local = upper;
            local_count = upper_count;
            shift_bs_local = tid;
        }
        // use barrier of the upper partner
        pthread_barrier_wait(bs_local + shift_bs_local);

        // receive data
        remote = recv[tid];
        remote_count = recv_counts[tid];

        // merge local and remote elements in place of new local subarray
        n = remote_count + local_count;
        free(ys);
        ys = (int*) malloc(n * sizeof(int));
        merge(ys, local, remote, local_count, remote_count);

        // this frees lower/upper and the pointers in recv
        free(local);
        free(remote);

        // iterate
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

    int** recv = (int**) malloc(T * sizeof(int*));     // arrays exchanged between threads
    int* recv_counts = (int*) malloc(T * sizeof(int)); // lenghts of arrays exchanged
    int* ps = (int*) malloc(T * sizeof(int));          // pivot element for each thread
    int* ns = (int*) malloc(T * sizeof(int));          // number of elements in each thread in xs

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

    static_args_t static_args = {N, T, recv, xs, recv_counts, ps, ns, bs_local, bs_group};
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
    for (unsigned char i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
    }

    // free shared memory
    free(recv);
    free(recv_counts);
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
        printf("Usage: ./quicksort N (input size) file (binary file containing integers) T (number of threads such that T = 2^k)\n");
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