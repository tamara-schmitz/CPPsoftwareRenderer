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

    inline T at ( std::size_t pos ) const
    {
        return T( deque.at( pos ) );
    }

    inline const size_t size()
    {
        return deque.size();
    }

    inline bool empty()
    {
        return deque.empty();
    }

    inline bool blocked()
    {
        return new_blocked;
    }

    inline bool isLast( size_t index )
    {
        return index >= deque.size();
    }

    bool isLastBlocked( size_t index )
    {
        std::unique_lock< std::mutex > lock( mutex );
        if ( isLast( index ) ) [[unlikely]]
        {
            while ( !blocked() )
                cond_islast.wait( lock );
            return isLast( index );
        } else { [[likely]]
            return false;
        }
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

    inline void unblock_new()
    {
        new_blocked = false;
    }

    void push_back( T& obj )
    {
        std::lock_guard< std::mutex > lock( mutex );
        if ( new_blocked ) [[unlikely]]
        {
            unblock_new();
        }
        deque.push_back( obj );
        cond_mod.notify_one();
    }

    unique_ptr< T > pop( )
    {
        // By default in blocking mode (waits for new objects if queue
        // is empty). However returns false if queue is
        // empty AND new_blocked == true.
        if ( blocked() ) {
            return nullptr;
        }

        std::unique_lock< std::mutex > lock( mutex );

        // wait until data arrives or queue is blocked
        while ( !blocked() && empty() )
        {
            cond_mod.wait( lock );
        }

        if ( blocked() || empty() ) {
            return nullptr;
        }

        unique_ptr< T > ret = make_unique<T> ( T( deque.front() ) );
        deque.pop_front();

        return ret;
    }

private:
    std::deque< T > deque = std::deque< T >();

    bool new_blocked = false;
    std::mutex mutex;
    std::condition_variable cond_mod;
    std::condition_variable cond_islast;
};

#endif // SAFEDEQUE_H
