#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
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
        for (int j = lo; j <= hi - 1; j++)
            if (xs[j] <= xs[hi])
                swap(&xs[++i], &xs[j]);
        swap(&xs[++i], &xs[hi]);

        // recursion
        serial_qs(xs, lo, i - 1);
        serial_qs(xs, i + 1, hi);
    }
}

/**
 * Merge two arrays ys and zs in a location on xs
*/
static void merge(int* xs, int* ys, int* zs, const int lo, const int size_y, const int size_z) {
    int i = lo, j = 0, k = 0;
    while (j < size_y && k < size_z) {
        if (ys[j] < zs[k]) {
            xs[i] = ys[j];
            j++;
        } else {
            xs[i] = zs[k];
            k++;
        }
        i++;
    }
    for (; j < size_y; j++) {
        xs[i] = ys[j];
        i++;
    }
    for (; k < size_z; k++) {
        xs[i] = zs[k];
        i++;
    }
}

/**
 * Use parallel quicksort on an array
 * as interpreted from QuickSort.pdf
*/
void parallel_qs(int* xs, const int N, const int T) {

    int** recv = (int**) malloc(T * sizeof(int*));     // arrays exchanged between threads
    int* recv_counts = (int*) malloc(T * sizeof(int)); // lenghts of arrays exchanged
    int* ps = (int*) malloc(T * sizeof(int));          // pivot element for each thread
    int* ns = (int*) malloc(T * sizeof(int));          // number of elements in each thread in xs
    int t = T / 2;                                     // threads per group

#pragma omp parallel shared(xs, ps, t, recv, recv_counts) firstprivate(N, T) num_threads(T)
{
    const int tid = omp_get_thread_num();
    // inclusive lower bound of this thread's subarray in xs
    int lo = tid * (N / T);
    // inclusive upper bound of this thread's subarray in xs
    int hi = tid < T - 1 ? (tid + 1) * (N / T) - 1 : N - 1;
    // total number of elements in this thread's subarray in xs
    int n = hi - lo + 1;

    // sort subarray locally
    serial_qs(xs, lo, hi);

    int p;             // value of pivot element
    int is_left_group; // if process is in a lower half group
    int n_prev;        // number of elements in xs before this process' elements

    // arrays for elements lower/higher than the pivot element
    int* lower = (int*) malloc(sizeof(int));
    int* upper = (int*) malloc(sizeof(int));
    int upper_count, lower_count;

    // arrays from elements received from another process and for retained elements
    int* remote, *local;
    int remote_count, local_count;

    // divide threads into groups until
    // no smaller groups can be formed
    while (t > 0) {

        // strategy 1
        // median of thread 0 of each group
        if (tid % (t * 2) == 0) {
            p = xs[(hi - lo) / 2 + lo];
            for (int i = tid; i < tid + (t * 2); i++)
                ps[i] = p;
        }
        #pragma omp barrier

        // count number of elements lower/higher than pivot element
        p = ps[tid];
        lower_count = 0;
        for (int i = lo; i <= hi; i++)
            if (xs[i] <= p)
                lower_count++;
        upper_count = n - lower_count;
        
        // do not use realloc!
        free(lower);
        free(upper);
        lower = (int*) malloc(lower_count * sizeof(int));
        upper = (int*) malloc(upper_count * sizeof(int));

        // split local subarray into lower/upper arrays
        int j = 0, k = 0;
        for (int i = lo; i <= hi; i++) {
            if (xs[i] <= p) {
                lower[j] = xs[i];
                j++;
            } else {
                upper[k] = xs[i];
                k++;
            }
        }

        /* Data exchange pattern:
         * T = 8, t = 4
         * left groups   right groups
         *   0 1 2 3   |   4 5 6 7
         * then
         * 0 <-> 4, 1 <-> 5, 2 <-> 6, etc. 
         */

        // check if thread is in a lower half group
        is_left_group = tid % (t * 2) < t;

        // send data
        if (is_left_group) {
            // send upper part, receive lower part
            recv[tid + t] = upper;
            recv_counts[tid + t] = upper_count;
            local = lower;
            local_count = lower_count;
        } else {
            // send lower part, receive upper part
            recv[tid - t] = lower;
            recv_counts[tid - t] = lower_count;
            local = upper;
            local_count = upper_count;
        }
        #pragma omp barrier

        // receive data
        remote = recv[tid];
        remote_count = recv_counts[tid];

        // reservate space for new local subarray in xs 
        n = remote_count + local_count;
        ns[tid] = n;
        #pragma omp barrier

        // compute new lower and upper bounds of local subarray
        n_prev = 0;
        for (int i = 0; i < tid; i++)
            n_prev += ns[i];
        lo = n_prev;
        hi = lo + n - 1;

        // merge local and remote elements in place of new local subarray in xs
        merge(xs, local, remote, lo, local_count, remote_count);

        // iterate
        #pragma omp single
        t /= 2;
    }

    free(lower);
    free(upper);
}
    free(recv);
    free(recv_counts);
    free(ps);
    free(ns);
}

int main(int argc, char** argv) {

    // parse input
    if (argc != 4) {
        printf("Usage: ./quicksort N (input size) file (binary file containing integers) T (number of threads such that T = 2^k)\n");
        exit(-1);
    }
    const int N = atoi(argv[1]);
    char* filename = (argv[2]);
    const int T = atoi(argv[3]);
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
    if ((T & (T - 1)) != 0) {
        printf("T must be defined such that T = 2^k\n");
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

    free(xs);

    return 0;
}