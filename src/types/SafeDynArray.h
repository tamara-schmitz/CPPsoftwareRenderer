#ifndef SAFEDYNARRAY_H
#define SAFEDYNARRAY_H

#include <queue>
#include <mutex>
#include <condition_variable>

template< class T >
class SafeDynArray
{
    // A wrapper for the C++ vector class.
    // Ensures thread safety through a mutex combined with
    // a lock guard.


public:

    SafeDynArray() {}

    void clear()
    {
        std::unique_lock< std::mutex > lock( mutex );
        dynarray.clear();
    }

    void block_new()
    {
        std::unique_lock< std::mutex > lock( mutex );
        new_blocked = true;
        lock.unlock();
        cond.notify_all();
    }

    void push_back( T obj )
    {
        std::unique_lock< std::mutex > lock( mutex );
        if ( new_blocked )
        {
            new_blocked = false;
        }
        dynarray.push_back( obj );
        lock.unlock();
        cond.notify_one();
    }

    T at ( std::size_t pos ) const
    {
        return dynarray.at( pos );
    }

    bool isLast( size_t index )
    {
        std::unique_lock< std::mutex > lock( mutex );
        if ( index >= dynarray.size() )
        {
            if ( new_blocked )
            {
                return true;
            }
            else
            {
                cond.wait( lock );
                if ( new_blocked )
                {
                    return true;
                }
            }
        }
        return false;
    }

    size_t size() const
    {
        return dynarray.size();
    }
private:
    std::vector< T > dynarray = std::vector< T >();
    bool new_blocked = false;
    std::mutex mutex;
    std::condition_variable cond;
};

#endif // SAFEDYNARRAY_H
