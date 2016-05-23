#pragma once

#include <Windows.h>

class Timer
{
private:

    bool initialized;
    LARGE_INTEGER frequency;

public:

    Timer();
    double AbsoluteTime();
};
