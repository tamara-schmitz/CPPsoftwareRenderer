#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template< class T >
class SafeQueue
{
    // A wrapper for the C++11 queue class.
    // Ensures thread safety through a mutex combined with
    // a lock guard.
    // new_blocked knows whether new elements are soon going to
    // be added.

    std::queue< T > queue;
    bool new_blocked;

public:

    SafeQueue() {}

    void push( T obj )
    {
        std::unique_lock< std::mutex > lock( mutex );
        if ( new_blocked )
        {
            new_blocked = false;
        }
        queue.push( obj );
        lock.unlock();
        cond.notify_one();
    }

    bool pop( T& obj )
    {
        // By default in blocking mode (waits for new objects if queue
        // is empty). However returns NULL if queue is
        // empty AND new_blocked == true.
        std::unique_lock< std::mutex > lock( mutex );
        if ( queue.empty() )
        {
            if ( new_blocked )
            {
                return false;
            }
            else
            {
                cond.wait( lock );
            }
        }
        obj = queue.front();
        queue.pop();

        return true;
    }
private:
    std::mutex mutex;
    std::condition_variable cond;
};

#endif // SAFEQUEUE_H
