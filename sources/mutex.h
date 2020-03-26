#include "thread_safety_annotations.hpp"

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace securefs
{
// Custom implementation of mutex.
// std::mutex is poorly implemented on certain platforms.
// In addition, we need thread safety annotations.
class THREAD_ANNOTATION_CAPABILITY("mutex") Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock() THREAD_ANNOTATION_ACQUIRE();
    void unlock() noexcept THREAD_ANNOTATION_RELEASE();

private:
#ifdef WIN32
    CRITICAL_SECTION m_lock;
#else
    pthread_mutex_t m_lock;
#endif
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
