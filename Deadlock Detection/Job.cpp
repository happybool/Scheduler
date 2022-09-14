/*
 *  Copyright (c) 2021, Aleksandr Don
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#include "Job.h"
#include <chrono>

cascade_mutex l0(0);

bool Scheduler::ScheduleJob(iJob* job)
{
	ScopedLock l01(l0, ScopedLock::Unique);
	if (job == 0 || scheduledJobs.size() >= threadLimit)
		return false;

	scheduledJobs.push_back(std::unique_ptr<iJob>(job));

	return true;
}

void Scheduler::BurstJobs()
{
	while (true)//normally provided with exit condition like a system event for exit
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		ScopedLock l01(l0, ScopedLock::Unique);
		for (const auto& job : scheduledJobs)
		{
			job->Run();
		}
		scheduledJobs.clear();
	}
}