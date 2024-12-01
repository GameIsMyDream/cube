#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

#include <cstdint>

YYZ_NAMESPACE_BEG

class Timer
{
public:
    Timer();

public:
    void Reset();
    void Start();
    void Stop();
    void Tick();
    float DeltaTime() const;
    float TotalTime() const;

private:
    double _seconds_per_count;
    double _delta_time;
    int64_t _base_time;
    int64_t _stop_time;
    int64_t _start_time;
    int64_t _paused_time;
    int64_t _curr_time;
    int64_t _prev_time;
    bool _stopped;
};

YYZ_NAMESPACE_END