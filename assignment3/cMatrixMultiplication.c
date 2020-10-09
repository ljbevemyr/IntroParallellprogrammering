#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <stdio.h>


static int ** allocate_a(int N) {
  int ** array;
  int i, j;
  array = malloc(N * sizeof(int*));
  for (i = 0; i < N ; i++)
    array[i] = malloc(N * sizeof(int));
  for (i = 0; i < N ; i++)
    for (j = 0; j < N ; j++)
      array[i][j] = i;
  return array;
}

static int ** allocate_b(int N) {
  int ** array;
  int i, j;
  array = malloc(N * sizeof(int*));
  for (i = 0; i < N ; i++)
    array[i] = malloc(N * sizeof(int));
  for (i = 0; i < N ; i++)
    for (j = 0; j < N ; j++)
      array[i][j] = j;
  return array;
}

static int ** allocate_c(int N) {
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

static void free_array(int ** array, int N) {
  int i;
  for (i = 0 ; i < N ; i++)
    free(array[i]);
  free(array);
}

int main(int argc, char *argv[])
{
  int dim = 1000;
  int **a = allocate_a(dim);
  int **b = allocate_b(dim);
  int **c = allocate_c(dim);

  double time;
  struct timeval ts, tf;

  gettimeofday(&ts,NULL);
#pragma omp parallel default(none) shared (a, b, c, dim)
   {         
       #pragma omp for schedule(static) collapse(3)
       for (int i = 0; i < dim; i++) {         
         for (int j = 0; j < dim; j++) {            
             for (int k = 0; k < dim; k++) {
               c[i][j] = 0;
               c[i][j] += a[i][k] * b[k][j];
             }
           }
       }      
   }
   gettimeofday(&tf, NULL);
   time = (tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;
   free_array(a, dim);
   free_array(b, dim);
   free_array(c, dim);
   printf("Runtime: %lf\n", time);
}
