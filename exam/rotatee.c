#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void rotate(int n, char *a, char first) {
  int i;
  int nthreads = omp_get_num_threads();
  int tid = omp_get_thread_num();

  int blocksize = (n-2)/nthreads;
  int start = tid*blocksize;
  int end = start + blocksize;

  if (tid == nthreads-1) end = n-2;

  char rv = a[end+1];
  #pragma omp barrier

  for (i=start; i<end; i++)
    a[i] = a[i+1];


  //printf("rv: %c\n", first);
  a[end] = rv;
  a[n-1] = first;
}

int main(int argc, char **argv) {
  int i;
  char *a;
  const int n=27;

  a = malloc(n * sizeof(char) );
  for (i=0; i<n-1; i++)
    a[i] = 'A'+i;

  a[n-1] = '\0';

  printf("Before: %s\n",a);

  #pragma omp parallel default(none) shared(a) private(i)
  {
    rotate(n, a, a[0]);
  }
  printf("After:  %s\n",a);

  return 0;
}
