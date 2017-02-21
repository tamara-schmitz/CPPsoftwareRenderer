#include "timer.h"

Timer::Timer()
{
    //ctor
}

Timer::~Timer()
{
    //dtor
}

// getters and setters
Uint32 Timer::GetFPSLimit()
{
    return fpsLimit;
}
void Timer::SetFPSLimit( Uint32 fps )
{
    fpsLimit = fps;
    if ( fps > 0 )
    {
        r_tickLimit = (1000.0 * 1000.0 * 1000.0) / (double) fps;
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

// wrappers
Uint64 Timer::GetHighResClockNs()
{
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}
void   Timer::nsSleep(Uint64 ns)
{
    #ifdef __unix__
        framesleep.tv_sec  = 0;
        framesleep.tv_nsec = ns;
        nanosleep( &framesleep, &sleepremains );
    #else
        std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
    #endif
}

// tick functions
void Timer::TickCall()
{
    // update maintime counter
    r_tickNow_main = GetHighResClockNs();
    // calculate maintime diff
    r_maintime_nano = r_tickNow_main - r_tickLast_main;

    // sleep if necessary
    if ( r_maintime_nano < r_tickLimit )
    {
        nsSleep( r_tickLimit - r_maintime_nano );
    }

    // update counters and vars
    r_tickLast_main = GetHighResClockNs();
    r_tickLast = r_tickNow;
    r_tickNow = GetHighResClockNs();
    // calculate frametime diff
    r_frametime_nano = r_tickNow - r_tickLast;
}
