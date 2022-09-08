/*
 *  Copyright (c) 2021, Aleksandr Don
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once
#include <shared_mutex>
#include <map>
#include <set>

//if locks are taken in order = ok
//if lock is taken out of order but belongs to current thread (meaning it was taken before) = ok
//if lock is taken more than once, then just skip it
//if lock is write, no read lock can be taken
//if lock ir read, other threads can take read locks
/// <summary>
/// Must be defined within class with Job function where it is suppose to be used as member variables
/// </summary>
class recursive_shared_prioritized_mutex : public std::shared_mutex
{
    


    inline static std::shared_mutex MutexID_Guard;
    /// <summary>
    /// maps MutexID to counter
    /// </summary>
    inline static thread_local std::map<short, short> MutexID_Map;
    /// <summary>
    /// maps MutexID to Mode it was originally taken in Unique = true, Shared = false
    /// used to decide how to unlock object based on how it was locked
    /// </summary>
    inline static thread_local std::map<short, bool> MutexMode_Map;
    std::atomic<size_t> owner;
    short MutexID = 0;
    bool CheckLocksOrder();
    //encapsulated overrides
    void lock(){}
    _NODISCARD bool try_lock_shared(){}
    _NODISCARD bool try_lock(){}
    void unlock_shared() {}
public:
    recursive_shared_prioritized_mutex(short UniqueID);
    void lock_unique();
    void unlock();
    void lock_shared();
};

/// <summary>
/// ScopedLock wraps mutex and allows creating scoped locks
/// it is recommended to use shared type locking at leaf on callstack
/// for instance when many threads need access resource at same time for reading
/// it implies that at time of reading thread cannot attempt writing to the resourse
/// </summary>
class ScopedLock
{
public:
    enum mode
    {
        Unique,
        SharedLeaf 
    };
private:
    recursive_shared_prioritized_mutex* _mut = 0;
    mode _mod = Unique;
public:
    ScopedLock(recursive_shared_prioritized_mutex& mutex, mode mod)
    {
        _mod = mod;
        _mut = &mutex;
        if (_mod == Unique)
            _mut->lock_unique();
        else
            _mut->lock_shared();
    }
    ~ScopedLock()
    {
        _mut->unlock();
    }
};