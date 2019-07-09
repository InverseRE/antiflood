/* -*- mode: c++ -*- */

/*
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

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

#include "scheduler.h"
#include "debug.h"

void Scheduler::setup(void)
{
    _time_mark = _ticker.mark();
}

bool Scheduler::add(Fptr task, unsigned long t2g)
{
    Task nt{task, t2g};
    Task* t = _tasks.get(nt);

    return !t && _tasks.add(nt);
}

bool Scheduler::drop(Fptr task)
{
    return _tasks.drop(Task{task, 0});
}

bool Scheduler::supress(Fptr task)
{
    Task* t = _tasks.get(Task{task, 0});

    return t && (t->t2g = -1);
}

bool Scheduler::force(Fptr task)
{
    Task* t = _tasks.get(Task{task, 0});

    return t && !(t->t2g = 0);
}

unsigned long Scheduler::run(void)
{
    const unsigned long mark = _ticker.mark();
    const unsigned long dt = mark - _time_mark;
    _time_mark = mark;

    for (Task* t = _tasks.start(); t; t = _tasks.next()) {
        t->t2g -= t->t2g < dt ? t->t2g : dt;
        t->t2g = t->t2g ? t->t2g : t->task(dt);
    }

    unsigned long next = -1;

    for (Task* t = _tasks.start(); t; t = _tasks.next()) {
        next = next < t->t2g ? next : t->t2g;
    }

    return next;
}
