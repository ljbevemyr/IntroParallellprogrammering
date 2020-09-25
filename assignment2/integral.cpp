#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>

std::mutex mutex;

float tot_sum = 0;

void calc_area(float start, float end) {
  float sum = 0;
  float start_value = 4/(1+pow(start, 2));
  float end_value = 4/(1+pow(end, 2));
  float width = end - start;
  float rect = (width) * end_value;
  float tri = (start_value - end_value) * width / 2;

  sum = rect + tri;
  mutex.lock();
  tot_sum += sum;
  mutex.unlock();
}

int main(int argc, char *argv[])
{
  int number_of_threads = 0;
  int number_of_traps = 0;

  std::cout << "Number of threads: ";
  std::cin >> number_of_threads;

  std::cout << "Number of trapezes: ";
  std::cin >> number_of_traps;

  std::thread *th = new std::thread[number_of_threads];

  float start = 0;
  float width = (float)1/number_of_traps;
  int k = 0;

  for (int i=0; i<number_of_traps; ++i) {
    float end = start + width;
    th[k] = std::thread(calc_area, start, end);
    start += width;
    k += 1;
    if (k >= number_of_threads) {

      for (int j=0; j<number_of_threads; j++) {
        th[j].join();
      }
      k = 0;
    }
  }

  for (int i=0; i<k; i++) {
    th[i].join();
  }

  std::cout << tot_sum << std::endl;
  delete[] th;
  return 0;
}
