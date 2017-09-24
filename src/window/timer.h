#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include <SDL2/SDL_stdinc.h>

// import unix/c++ library for highresclock and nanosleep
#ifdef __unix__
    #include <unistd.h>
#else
    #include <chrono>
    #include <thread>
#endif

class Timer
{
    public:
        Timer();
        virtual ~Timer();

        // getters and setters
        double GetFPSLimit() { return fpsLimit; }
        void   SetFPSLimit( double fps );
        Uint64 GetFrametime() { return r_frametime_nano; }
        Uint64 GetMaintime() { return r_maintime_nano; }
        double GetCurrentFPS();
        Uint64 GetDeltaTime() { return r_deltatime_nano; }
        Uint64 GetCurrentTick() { return r_currentTick; }
        void   SetDeltaLimits( Uint64 max_delta ); // in ms
        void   SetDeltaLimitsNs( Uint64 max_delta ); // in ns

        // tick functions
        void   TickCall(); // called whenever a tick is finished

        // pretty print
        void   printTimes();

    private:
        // usr vars
        double fpsLimit = 0; // limit to specified fps
        Uint64 r_tickLimit = 0;  // fpsLimit converted into ns
        // limits in ns for delta time
        Uint64 r_deltatimeLimit_max = -1; // maximum delta time

        // intern vars
        Uint64 r_tickLast       = 0;
        Uint64 r_tickNow        = 0;
        Uint64 r_tickLast_main  = 0;
        Uint64 r_tickNow_main   = 0;
        Uint64 r_currentTick    = 0; // tick count since init
        Uint64 r_frametime_nano = 0; // time between last and previous frames were shown
        Uint64 r_maintime_nano  = 0; // time required to actually render a frame
        Uint64 r_deltatime_nano = 0; // used for framerate independent movement
        int64_t r_sleepOverrun  = 0; // difference between expected sleep time and actual sleep time

        // time function wrappers
        Uint64 GetHighResClockNs(); // returns clock time in ns
        void   nsSleep( Uint64 ns ); // sleep for specified ns

};

#endif // TIMER_H
