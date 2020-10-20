#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void rotate(int n, double *a) {
  int thread_num = omp_get_thread_num();
  int thread_tot = omp_get_num_threads();
  int size = (n)/thread_tot;
  start
  start[0] = 0;
  for (i=0; i < thread_tot-1; i++) {
    end[i] = start[i] + size;
    start[i+1] = end[i];
  }
  end[thread_tot-1] = n;

  int s = start[thread_num];
  int e = end[thread_num];

  printf("N: %d, TOT: %d, Thread: %d, start: %d, end: %d\n", N, thread_tot, thread_num, s, e);

  for (int i = 0; i < n - 1; i++) {
    double tmp = a[i];
    a[i] = a[i+1];
    a[i+1] = tmp;
  }
}

static int ** allocate_array(int N) {
  int ** array;
  int i, j;
  array = malloc(N * sizeof(int*));
  for (i = 0; i < N ; i++)
    array[i] = malloc(N * sizeof(int));
  for (i = 0; i < N ; i++)
    for (j = 0; j < N ; j++)
      array[i][j] = 0;
  return array;
}

int main(int argc, char *argv[]) {
  int n = 20;
  int *a = allocate_array(n);
  a = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20];
  omp_set_num_threads(threads);
  #pragma omp parallel
  {
    rotate(n, a);
  }
}
