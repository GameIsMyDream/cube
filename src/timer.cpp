#include "timer.h"

#include <Windows.h>

YYZ_NAMESPACE_BEG

Timer::Timer() :
    _seconds_per_count(0.0),
    _delta_time(-1.0),
    _base_time(0),
    _stop_time(0),
    _start_time(0),
    _paused_time(0),
    _curr_time(0),
    _prev_time(0),
    _stopped(false)
{
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    _seconds_per_count = 1.0 / static_cast<double>(f.QuadPart);
}

void Timer::Reset()
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);

    _base_time = c.QuadPart;
    _prev_time = c.QuadPart;
    _stop_time = 0;
    _stopped = false;
}

void Timer::Start()
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    _start_time = c.QuadPart;

    if (_stopped)
    {
        _paused_time += (_start_time - _stop_time);
        _prev_time = _start_time;
        _stop_time = 0;
        _stopped = false;
    }
}

void Timer::Stop()
{
    if (!_stopped)
    {
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);

        _stop_time = c.QuadPart;
        _stopped = true;
    }
}

void Timer::Tick()
{
    if (_stopped)
    {
        _delta_time = 0.0;
        return;
    }

    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);

    _curr_time = c.QuadPart;
    _delta_time = (_curr_time - _prev_time) * _seconds_per_count;
    _prev_time = _curr_time;

    if (_delta_time < 0.0)
    {
        _delta_time = 0.0;
    }
}

float Timer::DeltaTime() const
{
    return static_cast<float>(_delta_time);
}

float Timer::TotalTime() const
{
    if (_stopped)
    {
        return static_cast<float>((_stop_time - _base_time - _paused_time) * _seconds_per_count);
    }
    else
    {
        return static_cast<float>((_curr_time - _base_time - _paused_time) * _seconds_per_count);
    }
}

YYZ_NAMESPACE_END
