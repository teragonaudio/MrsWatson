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

TaskTimer newTaskTimer(const CharString component, const char* subcomponent) {
  const char* componentCString = component != NULL ? component->data : NULL;
  return newTaskTimerWithCString(componentCString, subcomponent);
}

TaskTimer newTaskTimerWithCString(const char* component, const char* subcomponent) {
  TaskTimer taskTimer = (TaskTimer)malloc(sizeof(TaskTimerMembers));
#if WINDOWS
  LARGE_INTEGER queryFrequency;
#endif

  taskTimer->component = newCharStringWithCString(component);
  taskTimer->subcomponent = newCharStringWithCString(subcomponent);
  taskTimer->enabled = true;
  taskTimer->_running = false;
  taskTimer->totalTaskTime = 0.0;

#if WINDOWS
  QueryPerformanceFrequency(&queryFrequency);
  taskTimer->counterFrequency = (double)(queryFrequency.QuadPart) / 1000.0;
#endif

  return taskTimer;
}

void taskTimerStart(TaskTimer self) {
  if(self->_running) {
    taskTimerStop(self);
  }
#if WINDOWS
  QueryPerformanceCounter(&(self->startTime));
#elif UNIX
  gettimeofday(&self->startTime, NULL);
#endif
  self->_running = true;
}

double taskTimerStop(TaskTimer self) {
  double elapsedTimeInMs;
#if UNIX
  double elapsedFullSeconds;
  double elapsedMicroseconds;
  struct timeval currentTime;
#elif WINDOWS
  LONGLONG elapsedTimeInClocks;
  LARGE_INTEGER stopTime;
#endif

  if(!self->_running) {
    return 0.0;
  }

#if UNIX
  if(gettimeofday(&currentTime, NULL) == 0) {
    if(currentTime.tv_sec == self->startTime.tv_sec) {
      elapsedTimeInMs = (double)(currentTime.tv_usec - self->startTime.tv_usec) / 1000.0;
    }
    else {
      elapsedFullSeconds = (double)(currentTime.tv_sec - self->startTime.tv_sec - 1);
      elapsedMicroseconds = (double)(currentTime.tv_usec + (1000000l - self->startTime.tv_usec));
      elapsedTimeInMs = (elapsedFullSeconds * 1000.0) + (elapsedMicroseconds / 1000.0);
    }
    self->totalTaskTime += elapsedTimeInMs;
  }
#elif WINDOWS
  QueryPerformanceCounter(&stopTime);
  elapsedTimeInClocks = stopTime.QuadPart - self->startTime.QuadPart;
  elapsedTimeInMs = (double)(elapsedTimeInClocks) / self->counterFrequency;
  self->totalTaskTime += elapsedTimeInMs;
#endif

  self->_running = false;
  return elapsedTimeInMs;
}

CharString taskTimerHumanReadbleString(TaskTimer self) {
  int hours, minutes, seconds;
  CharString outString = newCharStringWithCapacity(kCharStringLengthShort);

  if(self->totalTaskTime < 1000) {
    snprintf(outString->data, outString->capacity, "%dms", (int)self->totalTaskTime);
  }
  else if(self->totalTaskTime < 60 * 1000) {
    seconds = (int)(self->totalTaskTime / 1000.0);
    snprintf(outString->data, outString->capacity, "%dsec", (int)seconds);
  }
  else {
    seconds = (int)(self->totalTaskTime / 1000.0) % 60;
    minutes = (int)(self->totalTaskTime / (1000.0 * 60.0));
    if(minutes > 60) {
      hours = minutes / 60;
      minutes = (minutes % 60);
      snprintf(outString->data, outString->capacity, "%d:%d:%dsec", hours, minutes, seconds);
    }
    else {
      snprintf(outString->data, outString->capacity, "%d:%dsec", minutes, seconds);
    }
  }

  return outString;
}

void freeTaskTimer(TaskTimer self) {
  if(self != NULL) {
    freeCharString(self->component);
    freeCharString(self->subcomponent);
    free(self);
  }
}
