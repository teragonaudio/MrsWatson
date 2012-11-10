//
// TaskTimer.c - MrsWatson
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

#include <stdio.h>
#include <stdlib.h>

#include "base/PlatformUtilities.h"
#include "time/TaskTimer.h"

#if UNIX
#include <sys/time.h>
#endif

TaskTimer newTaskTimer(const int numTasks) {
  TaskTimer taskTimer = (TaskTimer)malloc(sizeof(TaskTimerMembers));
  int i;
#if WINDOWS
  LARGE_INTEGER queryFrequency;
#endif

  taskTimer->numTasks = numTasks;
  taskTimer->currentTask = -1;
  taskTimer->totalTaskTimes = (double*)malloc(sizeof(double) * numTasks);
  for(i = 0; i < numTasks; i++) {
    taskTimer->totalTaskTimes[i] = 0.0;
  }
#if WINDOWS
  QueryPerformanceFrequency(&queryFrequency);
  taskTimer->counterFrequency = (double)(queryFrequency.QuadPart) / 1000.0;
#elif UNIX
  taskTimer->startTime = malloc(sizeof(struct timeval));
#endif

  return taskTimer;
}

void startTimingTask(TaskTimer taskTimer, const int taskId) {
  if(taskId == taskTimer->currentTask) {
    return;
  }
  stopTiming(taskTimer);
#if WINDOWS
  QueryPerformanceCounter(&(taskTimer->startTime));
#else
  gettimeofday(taskTimer->startTime, NULL);
#endif
  taskTimer->currentTask = taskId;
}

void stopTiming(TaskTimer taskTimer) {
#if WINDOWS
  LONGLONG elapsedTimeInClocks;
  if(taskTimer->currentTask >= 0) {
    LARGE_INTEGER stopTime;
    QueryPerformanceCounter(&stopTime);
    elapsedTimeInClocks = stopTime.QuadPart - taskTimer->startTime.QuadPart;
    taskTimer->totalTaskTimes[taskTimer->currentTask] += (double)(elapsedTimeInClocks) / taskTimer->counterFrequency;
  }
#else
  double elapsedTimeInMs;
  double elapsedFullSeconds;
  double elapsedMicroseconds;
  struct timeval currentTime;

  if(taskTimer->currentTask >= 0) {
    if(gettimeofday(&currentTime, NULL) == 0) {
      if(currentTime.tv_sec == taskTimer->startTime->tv_sec) {
        elapsedTimeInMs = (double)(currentTime.tv_usec - taskTimer->startTime->tv_usec) / 1000.0;
      }
      else {
        elapsedFullSeconds = (double)(currentTime.tv_sec - taskTimer->startTime->tv_sec - 1);
        elapsedMicroseconds = (double)(currentTime.tv_usec + (1000000l - taskTimer->startTime->tv_usec));
        elapsedTimeInMs = (elapsedFullSeconds * 1000.0) + (elapsedMicroseconds / 1000.0);
      }
      taskTimer->totalTaskTimes[taskTimer->currentTask] += elapsedTimeInMs;
    }
  }
#endif
  taskTimer->currentTask = -1;
}

void freeTaskTimer(TaskTimer taskTimer) {
  free(taskTimer->totalTaskTimes);
#if UNIX
  free(taskTimer->startTime);
#endif
  free(taskTimer);
}
