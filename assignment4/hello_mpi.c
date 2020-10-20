#include <stdio.h>
#include "mpi.h"
#include <stdbool.h>
#include <math.h>

/*int main(int argc, char *argv[]) {
  int rank, size, len;
  char name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(name, &len);

  printf("I am %d of %d\n", rank, size);
  printf("size=%d, rank=%d (%s)\n", size, rank, name);
  MPI_Finalize();
  return 0; }*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

struct arg_struct {
  int start;
  int end;
  int sqrt_chunk;
  int no_threads;
  int no_first_primes;
  int max;
  int first_primes[1];
};
int rank;

int my_atoi(char* str) {
  int i;
  int res = 0;
  for (i = 0; str[i] != '\0'; ++i)
    res = res * 10 + str[i] - '0';
  return res;
}

void first_calc_prime(bool *mark, int end) {
  int i, j;
  int k = 2;
  while ((k*k) <= end) {
    for (i=(k*k); i<=end; i=i+k) {
      mark[i] = true;
    }
    for (i=k+1; i<end; i++) {
      bool status = mark[i];
      if (status == false) {
        k = i;
        break;
      }
    }
  }
}

static void *chunk_calc_prime(void *args0) {
  struct arg_struct *args = (struct arg_struct *) args0;
  int *first_primes = args->first_primes;
  int no_first_primes = args->no_first_primes;
  int no_threads = args->no_threads;
  int max = args->max;
  int start = args->start;
  int end = args->end;
  int sqrt_chunk = args->sqrt_chunk;
  int size_of_chunk = ((max-sqrt_chunk)/no_threads) - 1;
  int i, j;
  int k = 2;
  int prime_start;
  bool *mark = malloc(sizeof(int)*(size_of_chunk+no_threads-1));

  for (i=0 ; i<size_of_chunk+no_threads-1 ; i++) {
    mark[i] = false;
  }

  //printf("rank: %d no_first_primes=%d\n", rank, no_first_primes);

  for (j=0 ; j<no_first_primes ; j++) {
    prime_start = 0;
    k = first_primes[j];

    for (i=start ; i <= end ; i++) {
      if (i%k == 0) {
        //printf("rank: %d i=%d k=%d\n", rank, i, k);
        prime_start = i;
        break;
      }
    }

    //printf("rank: %d prime_start = %d\n", rank, prime_start);

    if (prime_start != 0) {
      for (i=prime_start ; i <= end ; i=i+k) {
        //printf("rank: %d k: %d, prime_start: %d, start: %d, end: %d, "
        // "Not a prime: %d (marking %d)\n", rank, k, prime_start,
        // start, end, i, i-start);
        mark[i-start] = true;
      }
    }
  }

  /*for (i=start ; i <= end ; i++) {
    if (mark[i-start] == false)
      printf("found primes: %d\n", i);
      }*/


  return mark;
}




int main(int argc, char** argv) {
  int size, len, i;
  char name[MPI_MAX_PROCESSOR_NAME];

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(name, &len);

  if (argc != 2) {
    if (rank == 0) {
      printf("You need to provide one argument, which should be a max\n size=%d, rank=%d (%s)\n", size, rank, name);
      MPI_Finalize();
      return 0;
    }
    else{
      printf("size=%d, rank=%d (%s)\n", size, rank, name);
      MPI_Finalize();
      return 0;
    }
  }

  int max = atoi(argv[1]);
  struct arg_struct *args;
  int sqrt_chunk = sqrt(max);
  args = malloc(sizeof(struct arg_struct)+sqrt_chunk*sizeof(int));


  int no_threads = size;
  if (max < size) {
    no_threads = max;
  }
  max++;
  if (rank == 0) {
    bool mark[max+no_threads];
    for (i=0; i<max; i++) {
      mark[i] = false;
    }

    first_calc_prime(mark, sqrt_chunk);
    int no_first_primes = 0;
    for (i=2; i<sqrt_chunk; i++) {
      if (mark[i] == false) {
        args->first_primes[no_first_primes] = i;
        no_first_primes++;
      }
    }

    int size_of_chunks = ((max-sqrt_chunk)/no_threads) - 1;

    int start_of_chunks[no_threads];
    int end_of_chunks[no_threads];

    start_of_chunks[0] = sqrt_chunk+1;
    for (i=0; i < no_threads-2; i++) {
      end_of_chunks[i] = start_of_chunks[i] + size_of_chunks;
      start_of_chunks[i+1] = end_of_chunks[i] + 1;
    }
    end_of_chunks[no_threads-2] = max-1;
    printf("start of last chunk: %d, last number to calculate: %d\n", start_of_chunks[i], max-1);

    //printf("I am %d of %d\n", rank, size);
    printf("size=%d, rank=%d (%s)\n", size, rank, name);

    // We are assuming at least 2 processes for this task
    if (size < 2) {
      fprintf(stderr, "World size must be greater than 1 for %s", argv[0]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (i = 1 ; i<no_threads ; i++) {
      int number = 20;
      struct arg_struct *argsi = malloc(sizeof(int)*(size_of_chunks+no_threads-1));
      args->start = start_of_chunks[i-1];
      args->end = end_of_chunks[i-1];
      args->sqrt_chunk = sqrt_chunk;
      args->no_threads = no_threads;
      args->no_first_primes = no_first_primes;
      args->max = max;

      memcpy((char*)argsi, (char*)args, sizeof(int)*(size_of_chunks+no_threads-1));

      MPI_Send(
      /* data         = */ argsi,
      /* count        = */ sizeof(struct arg_struct)+sqrt_chunk*sizeof(int),
      /* datatype     = */ MPI_BYTE,
      /* destination  = */ i,
      /* tag          = */ 0,
      /* communicator = */ MPI_COMM_WORLD);

      //printf("2Process %d sent to %d\n", rank, i);
    }
    for (i=1 ; i<no_threads ; i++) {
      MPI_Recv(
      /* data         = */ &mark[start_of_chunks[i-1]],
      /* count        = */ sizeof(int)*(size_of_chunks+no_threads-1),
      /* datatype     = */ MPI_BYTE,
      /* source       = */ i,
      /* tag          = */ 0,
      /* communicator = */ MPI_COMM_WORLD,
      /* status       = */ MPI_STATUS_IGNORE);

    }
    if (rank == 0) {
      for (i=2; i<max; i++) {
        if (mark[i] == false) {
          printf("%d \0", i);
        }
      }
      printf("\n");
    }
  } else if (rank >= 1 && rank < no_threads) {
    MPI_Recv(
      /* data         = */ args,
      /* count        = */ sizeof(struct arg_struct)+sqrt_chunk*sizeof(int),
      /* datatype     = */ MPI_BYTE,
      /* source       = */ 0,
      /* tag          = */ 0,
      /* communicator = */ MPI_COMM_WORLD,
      /* status       = */ MPI_STATUS_IGNORE);
    //printf("Process %d received number %d from process 0\n", rank, args->first_primes[2]);

    bool *mark_chunk = chunk_calc_prime(args);

    int max = args->max;
    int sqrt_chunk = args->sqrt_chunk;
    int no_threads = args->no_threads;
    int size_of_chunks = ((max-sqrt_chunk)/no_threads) - 1;

    MPI_Send(
      /* data         = */ mark_chunk,
      /* count        = */ sizeof(int)*(size_of_chunks+no_threads-1),
      /* datatype     = */ MPI_BYTE,
      /* destination  = */ 0,
      /* tag          = */ 0,
      /* communicator = */ MPI_COMM_WORLD);

  }

  //printf("Before finalize: %d\n", rank);
  MPI_Finalize();
  //printf("After finalize: %d\n", rank);
  exit(0);
}
