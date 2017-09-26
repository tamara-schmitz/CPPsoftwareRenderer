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

    std::vector< T > dynarray;

public:

    SafeDynArray() {}

    void push_back( T obj )
    {
        std::unique_lock< std::mutex > lock( mutex );
        dynarray.push_back( obj );
        lock.unlock();
        cond.notify_one();
    }

    T at ( std::size_t pos ) const
    {
        return dynarray.at( pos );
    }
private:
    std::mutex mutex;
    std::condition_variable cond;
};

#endif // SAFEDYNARRAY_H
