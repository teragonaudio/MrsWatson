//
// TaskTimer.h - MrsWatson
// Created by Nik Reiman on 1/5/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef MrsWatson_TaskTimer_h
#define MrsWatson_TaskTimer_h

#include "base/CharString.h"
#include "base/PlatformUtilities.h"

#if UNIX
#include <sys/time.h>
#endif

typedef struct {
  CharString component;
  CharString subcomponent;
  boolByte enabled;
  boolByte _running;
  double totalTaskTime;

#if WINDOWS
  LARGE_INTEGER startTime;
  double counterFrequency;
#elif UNIX
  struct timeval startTime;
#endif
} TaskTimerMembers;
typedef TaskTimerMembers* TaskTimer;

/**
 * Create a new task timer.
 * @param component Component name which this timer belongs to. NULL or empty
 * string may be passed for this argument.
 * @param subcomponent Subcomponent which this timer is measured. NULL or empty
 * string may be passed for this argument.
 * @return Initialized instance
 */
TaskTimer newTaskTimer(const CharString component, const char* subcomponent);

/**
 * Create a new task timer.
 * @param component Component name which this timer belongs to. NULL or empty
 * string may be passed for this argument.
 * @param subcomponent Subcomponent which this timer is measured. NULL or empty
 * string may be passed for this argument.
 * @return Initialized instance
 */
TaskTimer newTaskTimerWithCString(const char* component, const char* subcomponent);

/**
 * Start the timer. Timers may be stopped and started multiple times.
 * @param self
 */
void taskTimerStart(TaskTimer self);

/**
 * Stop the timer. Timers may be stopped and started multiple times.
 * @param self
 * @return Time used since last call to taskTimerStart()
 */
double taskTimerStop(TaskTimer self);

/**
 * Get the string representation of the total accumulated time for this timer.
 * @param self
 * @return Formatted string, which the caller must free themselves when finished
 */
CharString taskTimerHumanReadbleString(TaskTimer self);

/**
 * Free a task timer and its associated resources
 * @param self
 */
void freeTaskTimer(TaskTimer self);

#endif
