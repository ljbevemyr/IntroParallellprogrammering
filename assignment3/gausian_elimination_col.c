#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main (int argc, char *argv[]) {
  //int n = 3;
  //int b[3] = {3, 1, 0};
  //int A[3][3] = { {2, -3, 0}, {0, 1, 1}, {0, 0, -5} };
  int n = 42000;
  int *b = calloc(n, sizeof(int));
  int **A = calloc(n, sizeof(int*));
  int *x = calloc(n, sizeof(int));
  int row, col, i;

  double time;//variables for timing
  struct timeval ts,tf;

  for (i = 0; i<n; i++) {
    A[i] = calloc(n, sizeof(int));
    A[i][i] = 1;
    b[i] = 1;
  }

  gettimeofday(&ts,NULL);

  #pragma omp parallel for schedule(auto)
  for (row = 0; row < n; row++)//OK
    x[row] = b[row];//OK

  for (col = n-1; col >= 0; col--) {
    x[col] /= A[col][col];


    #pragma omp parallel for schedule(auto)
    for (row = 0; row < col; row++) {
      x[row] -= A[row][col] * x[col];
    }
  }

  gettimeofday(&tf,NULL);
  time = (tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;
  printf("Gausian_col: Size %d Time %lf\n", n, time);

  /*for (i = 0; i < n; i++) {
    printf("x[%d] = %d\n", i, x[i]);
    }*/
  return 0;
}
