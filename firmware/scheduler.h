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

#include <list>

/** Task's metdadata. */
struct Task {
    unsigned long time2go;                  /**< time to go (remaining time before execution) */
    unsigned long (*task)(unsigned long dt); /**< pointer to a task */
}

/** Scheduler. */
class Scheduler {
private:
    const Ticker& _ticker;
    std::list<Task> _tasks;

public:
    Scheduler(const Ticker& ticker);
    void setup(void);

    bool add(unsigned long (*task)(unsigned long dt));
    bool drop(unsigned long (*task)(unsigned long dt));
    bool supress(unsigned long (*task)(unsigned long dt));
    bool force(unsigned long (*task)(unsigned long dt));
    unsigned long run(void);                /**< returns total time passed */
};

#endif  /* __SCHEDULER_H__ */
