#include <iostream>
#include <pthread.h>
#include <cmath>
#include <vector>
#include "omp.h"
#include <bits/stdc++.h>
using namespace std;

struct arg_struct {
  bool *mark;
  int start;
  int end;
  int sqrt_chunk;
};

int atoi(char* str) {
  int res = 0;
  for (int i = 0; str[i] != '\0'; ++i)
    res = res * 10 + str[i] - '0';
  return res;
}

void first_calc_prime(bool *mark, int end) {
 int k = 2;
 while (pow(k, 2) <= end) {
   for (int i=pow(k, 2); i<=end; i=i+k) {
     mark[i] = true;
   }
   for (int i=k+1; i<end; i++) {
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
  int start = args->start;
  int end = args->end;
  int sqrt_chunk = args->sqrt_chunk;
  bool *mark = args->mark;

  int k = 2;

  while (k <= sqrt_chunk) {
    if (mark[k] == false){
      int prime_start = 0;
      for (int i=start; i<=end; i++) {
        if (i%k == 0) {
          prime_start = i;
          break;
        }
      }
      for (int i=prime_start; i<=end; i=i+k) {
          mark[i] = true;
      }
    }
    k++;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "You need to provide two arguments, the first should be a max and the second number of threads" << std::endl;
    return 0;
  }
  int max = atoi(argv[1]);
  int no_threads = atoi(argv[2]);
  if (no_threads*4 > max) {
    std::cout << "There is no point in having this many threads. Provide a larger max or fewer number of threads." << std::endl;
    return 0;
  }
  max++;

  int sqrt_chunk = sqrt(max);

  bool *mark = new bool [max];
  for (int i=0; i<max; i++) {
    mark[i] = false;
  }

  first_calc_prime(mark, sqrt_chunk);

  //Fördelar intervallen
  int size_of_chunks = (max-sqrt_chunk)/no_threads;

  int *start_of_chunks = new int [no_threads];
  int *end_of_chunks = new int [no_threads];

  start_of_chunks[0] = sqrt_chunk+1;
  for (int i=0; i < no_threads-1; i++) {
    end_of_chunks[i] = start_of_chunks[i] + size_of_chunks;
    start_of_chunks[i+1] = end_of_chunks[i] + 1;
  }
  end_of_chunks[no_threads-1] = max-1;

  //Initiate threads
  struct arg_struct args[no_threads];

  for (int i=0 ; i<no_threads ; i++) {
    args[i].start = start_of_chunks[i];
    args[i].end = end_of_chunks[i];
    args[i].sqrt_chunk = sqrt_chunk;
    args[i].mark = mark;
  }

  omp_set_num_threads(no_threads);
  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    std::cout << "threadID: " << id << std::endl;
    chunk_calc_prime(&args[id]);
  }

/*for (int i=2; i<max; i++) {
    if (mark[i] == false) {
      std::cout << i << std::endl;
    }
    else {
    }
    }*/

return 0;
}
