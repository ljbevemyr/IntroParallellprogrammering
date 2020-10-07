/******************************************************
 ************* Conway's game of life ******************
 ******************************************************

 Usage: ./exec ArraySize TimeSteps

 Compile with -DOUTPUT to print output in output.gif
 (You will need ImageMagick for that - Install with
  sudo apt-get install imagemagick)
 WARNING: Do not print output for large array sizes!
          or multiple time steps!
 ******************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define FINALIZE "\
convert -delay 20 out*.pgm output.gif\n\
rm *pgm\n\
"

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

static void free_array(int ** array, int N) {
  int i;
  for (i = 0 ; i < N ; i++)
    free(array[i]);
  free(array);
}

static void init_random(int ** array1, int ** array2, int N) {
  int i, pos;

  for (i = 0 ; i < (N * N)/10 ; i++) {
    pos = rand() % ((N-2)*(N-2));
    array1[pos%(N-2)+1][pos/(N-2)+1] = 1;
    array2[pos%(N-2)+1][pos/(N-2)+1] = 1;

  }
}

#ifdef OUTPUT
static void print_to_pgm(int ** array, int N, int t) {
  int i, j;
  char * s = malloc(30*sizeof(char));
  sprintf(s, "out%d.pgm", t);
  FILE * f = fopen(s, "wb");
  fprintf(f, "P5\n%d %d 1\n", N,N);
  for (i = 0; i < N ; i++)
    for (j = 0; j < N ; j++)
      if (array[i][j] == 1)
        fputc(1, f);
      else
        fputc(0, f);
  fclose(f);
  free(s);
}
#endif

static void tick_thread(int T, int N, int ** current, int ** previous) {
  int t, i, j, nbrs;
  int thread_num = omp_get_thread_num();
  int thread_tot = omp_get_num_threads();
  int tot_elem = (N-2)*(N-2) / thread_tot; //så många element tråden ska räkna
  int start_v, start_h, end_v, end_h;
  /*int start_horizontal = ((thread_num * tot_elements) / N) + 1;
  int start_vertical = ((thread_num * tot_elements) % N) + 1;
  int end_horizontal, end_vertical;
  if (thread_num == thread_tot) {
    end_horizontal = N-1;
    end_vertical = N-1;
  }
  else{
    end_horizontal = ((thread_num+1) * tot_elements / N)+1;// ((0+1)*16/10)+1 = 2
    end_vertical = ((thead_num+1) * tot_elements) % N;// ((0+1)*16%10) = 6
    }*/

  if (thread_num == 0) {
    start_v = 1;
    start_h = 1;
    end_v = (tot_elem / (N-2)) + 1;
    end_h = (tot_elem % (N-2));
  }
  else if (thread_num == thread_tot) {
    start_v = (tot_elem*(thread_num) / (N-2)) + 1;
    start_h = (tot_elem*(thread_num) % (N-2));
    end_v = N-1;
    end_h = N-1;
  }
  else {
    start_v = (tot_elem*(thread_num) / (N-2)) + 1;
    start_h = (tot_elem*(thread_num) % (N-2));
    end_v = (tot_elem*(thread_num+1) / (N-2)) + 1;
    end_h = (tot_elem*(thread_num+1) % (N-2));
  }
  for (t = 0 ; t < T ; t++) { //time steps
    for (i = start_v ; i < end_v ; i++) //array dimensions
      for (j = start_h ; j < end_h ; j++) { //array dimensions
        nbrs = previous[i+1][j+1] + previous[i+1][j] + previous[i+1][j-1] \
          + previous[i][j-1] + previous[i][j+1] \
          + previous[i-1][j-1] + previous[i-1][j] + previous[i-1][j+1];
        if (nbrs == 3 || ( previous[i][j]+nbrs == 3))
          current[i][j] = 1;
        else
          current[i][j] = 0;
      }
    #ifdef OUTPUT
    print_to_pgm(current, N, t+1);
    #endif
    //Swap current array with previous array
    int ** swap = current;
    current = previous;
    previous = swap;
    #pragma omp barrier
  }

}

int main (int argc, char * argv[]) {
  int N; //array dimensions
  int T; //time steps
  int ** current, ** previous; //arrays - one for current timestep, one for previous timestep
  //int ** swap;//array pointer
  //int t, i, j, nbrs;//helper variables
  int threads;

  double time;//variables for timing
  struct timeval ts,tf;

  /*Read input arguments*/
  if (argc != 4) {
    fprintf(stderr, "Usage: ./exec ArraySize TimeSteps\n");
    exit(-1);
  }
  else {
    N = atoi(argv[1]);
    T = atoi(argv[2]);
    threads = atoi(argv[3]);
  }

  /*Allocate and initialize matrices*/
  current = allocate_array(N);//allocate array for current time step
  previous = allocate_array(N);//allocate array for previous time step

  init_random(previous, current, N);//initialize previous array with pattern

#ifdef OUTPUT
  print_to_pgm(previous, N, 0);
#endif

  /*Game of Life*/

  gettimeofday(&ts,NULL);
  /*for (t = 0 ; t < T ; t++) { //time steps
    for (i = 1 ; i < N-1 ; i++) //array dimensions
      for (j = 1 ; j < N-1 ; j++) { //array dimensions
        nbrs = previous[i+1][j+1] + previous[i+1][j] + previous[i+1][j-1] \
          + previous[i][j-1] + previous[i][j+1] \
          + previous[i-1][j-1] + previous[i-1][j] + previous[i-1][j+1];
        if (nbrs == 3 || ( previous[i][j]+nbrs == 3))
          current[i][j] = 1;
        else
          current[i][j] = 0;
      }

#ifdef OUTPUT
    print_to_pgm(current, N, t+1);
#endif
    //Swap current array with previous array
    swap = current;
    current = previous;
    previous = swap;

    }*/

  
  gettimeofday(&tf,NULL);
  time = (tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;

  free_array(current, N);
  free_array(previous, N);
  printf("GameOfLife: Size %d Steps %d Time %lf\n", N, T, time);
#ifdef OUTPUT
  system(FINALIZE);
#endif
  return 0;
}

