#include "thread_safety_annotations.hpp"

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

private:
    std::mutex mut_;
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
