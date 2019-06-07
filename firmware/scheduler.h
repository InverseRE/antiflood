/* -*- mode: c++ -*- */

/*
   Antiflood Copyright (C) 2018 Alexey <Inverse> Shumeiko

   This file is part of Antiflood project.

   This firmware is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This firmware is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the firmware. If not, see <http://www.gnu.org/licenses/>.

   For more details see LICENSE file.
*/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "list.h"
#include "ticker.h"

/** Task pointer. */
typedef unsigned long (*Fptr)(unsigned long dt);

/** Task's metdadata. */
struct Task {
    unsigned long t2g;                      /**< time to go (remaining time before execution) */
    Fptr task;                              /**< pointer to a task */

    bool operator ==(const Task& t) const { return task == t.task; }
};

/** Scheduler. */
class Scheduler {
private:
    const Ticker& _ticker;
    unsigned long _time_mark;
    List<Task> _tasks;

public:
    Scheduler(const Ticker& ticker) : _ticker(ticker) {}
    void setup(void);

    bool add(Fptr task, unsigned long t2g = 0);
    bool drop(Fptr task);
    bool supress(Fptr task);
    bool force(Fptr task);
    unsigned long run(void);                /**< returns total time passed */
};

#endif  /* __SCHEDULER_H__ */
