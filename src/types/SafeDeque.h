#ifndef SAFEDEQUE_H
#define SAFEDEQUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template< class T >
class SafeDeque
{
    // A wrapper for the C++11 deque class.
    // Ensures thread safety through a mutex combined with
    // a lock guard.
    // new_blocked knows whether new elements are soon going to
    // be added.

public:

    SafeDeque() {}

    T at ( std::size_t pos ) const
    {
        return deque.at( pos );
    }

    size_t size() const
    {
        return deque.size();
    }

    bool isLast( size_t index )
    {
        std::unique_lock< std::mutex > lock( mutex );
        if ( index >= deque.size() )
        {
            while ( !new_blocked )
                cond_islast.wait( lock );
            return true;
        }

        return false;
    }


    void clear()
    {
        std::unique_lock< std::mutex > lock( mutex );
        deque.clear();
        lock.unlock();
        cond_mod.notify_all();
    }

    void block_new()
    {
        std::lock_guard< std::mutex > lock( mutex );
        new_blocked = true;
        cond_mod.notify_all();
        cond_islast.notify_all();
    }

    void unblock_new()
    {
        new_blocked=false;
    }
    
    void push_back( T& obj )
    {
        std::lock_guard< std::mutex > lock( mutex );
        if ( new_blocked )
        {
            unblock_new();
        }
        deque.push_back( obj );
        cond_mod.notify_one();
    }

    bool pop( T& obj )
    {
        // By default in blocking mode (waits for new objects if queue
        // is empty). However returns false if queue is
        // empty AND new_blocked == true.

        if ( new_blocked && deque.empty() )
        {
            return false;
        }

        std::unique_lock< std::mutex > lock( mutex );

        // wait until data arrives or queue is blocked
        while ( !new_blocked && deque.empty() )
        {
            cond_mod.wait( lock );
        }

        if ( new_blocked )
            return false;

        obj = T(deque.front());
        deque.pop_front();

        return true;
    }

private:
    std::deque< T > deque = std::deque< T >();

    bool new_blocked = false;
    std::mutex mutex;
    std::condition_variable cond_mod;
    std::condition_variable cond_islast;
};

#endif // SAFEDEQUE_H
