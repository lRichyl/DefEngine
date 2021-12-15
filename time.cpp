#include "time.h"
#include <iostream>
#include <windows.h>

float Timer::dt = 0;
void Timer::Tick(){
     if(isTimeReached || isStopped){
          timeCount = 0;
          isTimeReached = false;
     }
     isStopped = false;
     if(!isPaused){
          timeCount += dt;
     }
     // printf("%f\n",timeCount);
     if(timeCount >= timeToWait){
          isTimeReached = true;
     }
}

void Timer::Pause(){
     isPaused  = true;
}
void Timer::Stop(){
     isPaused = true;
     isStopped = true;
     isTimeReached = false;
     timeCount = 0;
}
// void Timer::Reset(){
//      Stop();
//      isTimeReached = false;
//      timeCount = 0;
// }

void Timer::Resume(){
     isPaused = false;
     isStopped = false;
}

long long get_performance_counter_frequency(){
     LARGE_INTEGER perf_count_frequency_result;
     QueryPerformanceFrequency(&perf_count_frequency_result);
     long long perf_count_frequency = perf_count_frequency_result.QuadPart;
     return perf_count_frequency;
}

LARGE_INTEGER get_time_counter(){
     LARGE_INTEGER counter;
     QueryPerformanceCounter(&counter);
     return counter;
}

float get_time_in_ms(LARGE_INTEGER start_counter, LARGE_INTEGER end_counter, long long perf_count_frequency, int *fps){
     long long counter_elapsed = end_counter.QuadPart - start_counter.QuadPart;
     float time_in_ms = ((1000*(float)counter_elapsed) / (float)perf_count_frequency);
     if(fps != nullptr){
          *fps = perf_count_frequency / counter_elapsed;
     }
     //last_counter = end_counter;
     return time_in_ms;
}
