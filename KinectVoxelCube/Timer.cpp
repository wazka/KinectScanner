#include "Timer.h"

Timer::Timer()
{
    if (QueryPerformanceFrequency(&frequency) == 0)
    {
        initialized = false;
        return;
    }

    initialized = true;
}

double Timer::AbsoluteTime()
{
    if (initialized == false)
        return 0;

    LARGE_INTEGER systemTime;
    if (QueryPerformanceCounter(&systemTime) == 0)
        return 0;

    double realTime = (double)systemTime.QuadPart / (double)frequency.QuadPart;
    return realTime;
}
