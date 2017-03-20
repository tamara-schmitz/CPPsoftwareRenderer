#include "timer.h"
#include <iostream>

Timer::Timer()
{
    //ctor
}

Timer::~Timer()
{
    //dtor
}

// getters and setters
double Timer::GetFPSLimit()
{
    return fpsLimit;
}
void Timer::SetFPSLimit( double fps )
{
    fpsLimit = fps;
    if ( fps > 0 )
    {
        r_tickLimit = (1000.0 * 1000.0 * 1000.0) / fps;
    }
    else
    {
        r_tickLimit = 0;
    }

}
Uint64 Timer::GetFrametime()
{
    return r_frametime_nano;
}
Uint64 Timer::GetMaintime()
{
    return r_maintime_nano;
}
double Timer::GetCurrentFPS()
{
    if ( r_frametime_nano > 0 )
    {
        return (1000.0 * 1000.0 * 1000.0) / r_frametime_nano;
    }
    else
    {
        return 10000.0;
    }
}

// clock wrapper
Uint64 Timer::GetHighResClockNs()
{
    #ifdef __unix__
        struct timespec time;
        clock_gettime( CLOCK_MONOTONIC, &time );
        return time.tv_sec * 1000 * 1000 * 1000 + time.tv_nsec;
    #else
        return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
    #endif
}
void   Timer::nsSleep(Uint64 ns)
{
    #ifdef __unix__
        struct timespec framesleep,sleepremains;
        framesleep.tv_sec  = 0;
        framesleep.tv_nsec = ns;
        nanosleep( &framesleep, &sleepremains );
    #else
        std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
    #endif
}

// tick function
void Timer::TickCall()
{
    // update maintime counter
    r_tickNow_main = GetHighResClockNs();
    // calculate maintime diff
    r_maintime_nano = r_tickNow_main - r_tickLast_main;

    // sleep if necessary
    if ( r_maintime_nano < r_tickLimit )
    {
        nsSleep( r_tickLimit - r_maintime_nano - r_sleepOverrun );
    }

    // update counters and vars
    r_tickLast_main = GetHighResClockNs();
    r_tickLast = r_tickNow;
    r_tickNow  = GetHighResClockNs();
    // calculate frametime diff and sleep overrun
    r_frametime_nano = r_tickNow - r_tickLast;
    r_sleepOverrun  = r_frametime_nano - ( r_tickLimit - r_sleepOverrun * 0.966 );
    if ( r_sleepOverrun < 0 )
    {
        r_sleepOverrun = 0;
    }
}

// pretty print
void Timer::printTimes()
{
    std::cout << "Framerate: "  << GetCurrentFPS()
              << " Frametime: " << GetFrametime() / ( 1000.0f * 1000.0f ) << " ms"
              << " Maintime: "  << GetMaintime()  / ( 1000.0f * 1000.0f ) << " ms"
              << " Sleepoverun: " << r_sleepOverrun / ( 1000.0f * 1000.0f ) << " ms"
              << std::endl;
}
