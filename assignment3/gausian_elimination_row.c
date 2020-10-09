#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main (int argc, char *argv[]) {
  int n = 42000;
  int *b = calloc(n, sizeof(int));
  int **A = calloc(n, sizeof(int*));
  int *x = calloc(n, sizeof(int));
  int row, col, red, i;

  double time;//variables for timing
  struct timeval ts,tf;

  for (i = 0; i<n; i++) {
    A[i] = calloc(n, sizeof(int));
    A[i][i] = 1;
    b[i] = 1;
  }

  gettimeofday(&ts,NULL);

  for (row = n-1; row >= 0; row--) {
    x[row] = b[row];
    red = x[row];
    #pragma omp parallel for reduction(-:red)
    for (col = row+1; col < n; col++) {
      red -= A[row][col] * x[col];
      //printf("threadID: %d\n", omp_get_thread_num());
    }
    x[row] = red/A[row][row];
  }

  gettimeofday(&tf,NULL);
  time = (tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;
  printf("Gausian_col: Size %d Time %lf\n", n, time);

  for (i = 0; i < 3; i++) {
    printf("x[%d] = %d\n", i, x[i]);
  }
  return 0;
}
