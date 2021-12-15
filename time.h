#ifndef TIME_H
#define TIME_H
#include <windows.h>

struct Timer{
     static float dt;
     float timeCount = 0;
     float timeToWait = 0;
     bool isTimeReached = false;
     bool isPaused  = false;
     bool isStopped = false;

     void Tick();
     void Pause();
     void Stop();
     void Resume();
     // void Reset();
};


long long get_performance_counter_frequency();
LARGE_INTEGER get_time_counter();
float get_time_in_ms(LARGE_INTEGER start_counter, LARGE_INTEGER end_counter, long long perf_count_frequency, int *fps = nullptr);


#endif
