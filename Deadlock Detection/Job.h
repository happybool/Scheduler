/*
 *  Copyright (c) 2021, Aleksandr Don
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once
#include <functional>
#include <vector>
#include "guard.h"
#include <thread>

class iJob
{
public:
	virtual void Run() = 0;
	virtual ~iJob() {};
};

template<typename F, typename... Args>
class Job : public iJob
{
	std::string _name;
	std::function<F(/*std::atomic<bool> stopThread, */ Args...arguments)> _func;
public:
	/// <summary>
	/// Templated Job, takes parameter packed function
	/// arguments can be anything in any quantity
	/// use std::bind to bind a specific class with function
	/// see https://en.cppreference.com/w/cpp/utility/functional/bind
	/// </summary>
	Job(std::function<F(/*std::atomic<bool> stopThread, */ Args...arguments)> func, std::string name) : _func(func), _name(name) {}

	void Run()
	{
		std::thread th(_func);
		th.detach();
	}
private:
	Job(const Job<F> & other) {};
	Job() {};
};
/// <summary>
/// Object of this class can run separete jobs
/// Small and Big, One time or infinitely looping
/// Dont forget to pass stopping semaphore 
/// </summary>
class Scheduler
{
	unsigned short threadLimit = 100;
	std::vector<std::unique_ptr<iJob>> scheduledJobs;
public:
	Scheduler()
	{
		std::thread th(std::bind(&Scheduler::BurstJobs, this));
		th.detach();
	}
	/// <summary>
	/// Method queues a Job for execution
	/// you dont need to worry about removing job pointer from heap
	/// it uses "smart pointers" https://en.cppreference.com/book/intro/smart_pointers
	/// </summary>
	/// <param name="job"></param>
	bool ScheduleJob(iJob* job);
	void BurstJobs();
};