#pragma once
#include "exceptions.h"
#include "thread_safety_annotations.hpp"

#include <chrono>
#include <errno.h>
#include <mutex>

namespace securefs
{
class THREAD_ANNOTATION_CAPABILITY("mutex") Mutex
{
public:
    void lock() THREAD_ANNOTATION_ACQUIRE() { mut_.lock(); }
    void lock_shared() THREAD_ANNOTATION_ACQUIRE_SHARED() { mut_.lock(); }
    void unlock() THREAD_ANNOTATION_RELEASE() { mut_.unlock(); }
    void unlock_shared() THREAD_ANNOTATION_RELEASE_SHARED() { mut_.unlock(); }

    // Needed for negative capabilities to work
    Mutex& operator!() { return *this; };

private:
    std::mutex mut_;
};

class THREAD_ANNOTATION_CAPABILITY("mutex") TimedMutex
{
public:
    void lock() THREAD_ANNOTATION_ACQUIRE()
    {
        if (!mut_.try_lock_for(std::chrono::seconds(10)))
        {
            throwVFSException(EBUSY);
        }
    }

    void unlock() THREAD_ANNOTATION_RELEASE() { mut_.unlock(); }

    // Needed for negative capabilities to work
    TimedMutex& operator!() { return *this; };

private:
    std::timed_mutex mut_;
};

template <class Lockable>
class THREAD_ANNOTATION_SCOPED_CAPABILITY LockGuard
{
private:
    Lockable* lockable_;

public:
    explicit LockGuard(Lockable& lockable) THREAD_ANNOTATION_ACQUIRE(lockable)
        : lockable_(&lockable)
    {
        lockable_->lock();
    }
    ~LockGuard() THREAD_ANNOTATION_RELEASE() { lockable_->unlock(); }
};

template <class Lockable>
class THREAD_ANNOTATION_SCOPED_CAPABILITY SharedLockGuard
{
private:
    Lockable* lockable_;

public:
    explicit SharedLockGuard(Lockable& lockable) THREAD_ANNOTATION_ACQUIRE_SHARED(lockable)
        : lockable_(&lockable)
    {
        lockable_->lock_shared();
    }
    ~SharedLockGuard() THREAD_ANNOTATION_RELEASE() { lockable_->unlock(); }
};
}    // namespace securefs
