#include "timeMan.h"
#include <iomanip>
#include <iostream>

double TimeMan::GetClock()
{
#if defined(_MSC_VER)
    return clock();
#else
    struct timeval val;
    struct timezone zone;
    if (gettimeofday(&val, &zone) == -1)
    {
        std::cerr << "time err" << std::endl;
        exit(0);
    }
    double t = val.tv_sec * 1000.0 + (val.tv_usec / 1000.0);
    return t;
#endif
}

double TimeMan::GetSpendTime(double ct)
{
    //int div = CLOCKS_PER_SEC;	// 1000 ...VC, 1000000 ...gcc
    int div = 1000;
    return (double)(GetClock() + 1 - ct) / div;
}

int TimeMan::IsTimeOver()
{
    if (GetSpendTime(start_time) >= time_limit_sec)
        return 1;

    return 0;
}

void TimeMan::SetUpStartTime(double total_time)
{
    double base_time = 60 * 10; // 10 minutes
    double left_time = base_time - total_time;
    int div = 12; // 40 ... 13x13, 70 ... 19x19
    time_limit_sec = left_time / div;
    if (left_time < 60)
        time_limit_sec = 1.0;
    if (left_time < 20)
        time_limit_sec = 0.2;

    std::cerr << "time_limit_sec=" << std::setprecision(1) << time_limit_sec
        << ", total=" << std::setprecision(1) << total_time << ", left=" << std::setprecision(1) << left_time << std::endl;

    // 開始時刻
    start_time = GetClock();
}
