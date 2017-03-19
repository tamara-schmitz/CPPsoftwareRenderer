#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL_stdinc.h>
#include <chrono>
#include <thread>

class Timer
{
    public:
        Timer();
        virtual ~Timer();

        // getters and setters
        Uint32 GetFPSLimit();
        void   SetFPSLimit( Uint32 fps );
        Uint64 GetFrametime();
        Uint64 GetMaintime();
        double GetCurrentFPS();

        // tick functions
        void   TickCall(); // called whenever a tick is finished

        // pretty print
        void   printTimes();

    private:
        // usr vars
        Uint32 fpsLimit = 0; // limit to specified fps
        Uint64 r_tickLimit = 0;  // fpsLimit converted into ns

        // intern vars
        Uint64 r_tickLast       = 0;
        Uint64 r_tickNow        = 0;
        Uint64 r_tickLast_main  = 0;
        Uint64 r_tickNow_main   = 0;
        Uint64 r_frametime_nano = 0; // time between last and previous frames were shown
        Uint64 r_maintime_nano  = 0; // time required to actually render a frame

        // unix struct
        struct timespec framesleep,sleepremains;

        // time function wrappers
        Uint64 GetHighResClockNs(); // returns clock time in ns
        void   nsSleep( Uint64 ns ); // sleep for specified ns

};

#endif // TIMER_H
