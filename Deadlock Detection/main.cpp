#pragma once
#include <iostream>
#include "job.h"
#include "guard.h"

recursive_shared_prioritized_mutex l1(1);
recursive_shared_prioritized_mutex l2(2);
recursive_shared_prioritized_mutex l3(3);

class SimulatedDeadlock
{
public:
    int add(int a, int b)
    {
        //Normal operation Locks are taken in order
        ScopedLock l01(l1, ScopedLock::Unique);
        ScopedLock l02(l2, ScopedLock::SharedLeaf);
        ScopedLock l03(l3, ScopedLock::Unique);
        return a + b;
    }
    int sub(int a, int b)
    {
        //Must throw exception due to reverce order
        ScopedLock l02(l2, ScopedLock::Unique);
        ScopedLock l01(l1, ScopedLock::Unique); 
        return a / b;
    }
    int div(int a, int b)
    {
        //Must throw exception due to Unigue cannot be taken after SharedLeaf
        ScopedLock l02(l1, ScopedLock::SharedLeaf);
        ScopedLock l01(l1, ScopedLock::Unique);
        return a - b;
    }
    int mul(int a, int b)
    {
        return a * b;
    }
};

int main()
{
    //Example of Job scheduling
    Scheduler sched;
    auto s = new SimulatedDeadlock();
    //Job constructs from binding to method of a class
    auto j1 = new Job<int>(std::bind(&SimulatedDeadlock::add, s, 1, 2), "Test1");
    auto j2 = new Job<int>(std::bind(&SimulatedDeadlock::sub, s, 2, 3), "Test2");
    auto j3 = new Job<int>(std::bind(&SimulatedDeadlock::div, s, 3, 4), "Test3");
    sched.ScheduleJob(j1);
    sched.ScheduleJob(j2);
    sched.ScheduleJob(j3);
    getchar();
}