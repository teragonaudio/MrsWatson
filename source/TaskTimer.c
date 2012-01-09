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
#include <sys/time.h>
#include "TaskTimer.h"

TaskTimer newTaskTimer(const int numTasks) {
  TaskTimer taskTimer = malloc(sizeof(TaskTimerMembers));

  taskTimer->numTasks = numTasks;
  taskTimer->currentTask = -1;
  taskTimer->totalTaskTimes = malloc(sizeof(unsigned long) * numTasks);
  for(int i = 0; i < numTasks; i++) {
    taskTimer->totalTaskTimes[i] = 0;
  }
  taskTimer->startTime = malloc(sizeof(struct timeval));

  return taskTimer;
}

void startTimingTask(TaskTimer taskTimer, const int taskId) {
  if(taskId == taskTimer->currentTask) {
    return;
  }
  stopTiming(taskTimer);
  gettimeofday(taskTimer->startTime, NULL);
  taskTimer->currentTask = taskId;
}

void stopTiming(TaskTimer taskTimer) {
  if(taskTimer->currentTask >= 0) {
    struct timeval currentTime;
    if(gettimeofday(&currentTime, NULL) == 0) {
      const long elapsedTimeInMs = ((currentTime.tv_sec - (taskTimer->startTime->tv_sec + 1)) * 1000) +
        (currentTime.tv_usec / 1000) + (1000 - (taskTimer->startTime->tv_usec / 1000));
      taskTimer->totalTaskTimes[taskTimer->currentTask] += elapsedTimeInMs;
    }
  }
}

void freeTaskTimer(TaskTimer taskTimer) {
  free(taskTimer->totalTaskTimes);
  free(taskTimer->startTime);
  free(taskTimer);
}
