#include <iostream>
#include <omp.h>

int main(int argc, char *argv[])
{
  int dim = 5;
  int a[dim][dim] {};
  int b[dim][dim] {};
  int c[dim][dim] {};
  
  for (int i; i<dim; i++){
    for (int j; j<dim; j++){
      a[i][j] = i;
      b[i][j] = j;
      c[i][j] = 0;
    }
  }
  
  #pragma omp parallel default(none) shared (a, b, c, dim) num_threads(4)
   {         
     #pragma omp for schedule(static)
       for (int i = 0; i < dim; i++) {
         for (int j = 0; j < dim; j++) {
            c[i][j] = 0;
             for (int k = 0; k < dim; k++) {
               c[i][j] += a[i][k] * b[k][j];
             }
           }
       }      
   }
}
