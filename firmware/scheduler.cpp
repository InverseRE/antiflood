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

#include "scheduler.h"

std::list<Task>::iterator Scheduler::find(Fptr task) const
{
    return std::find_if(_tasks.begin(), _tasks.end(),
            [&task](const Task v) { return v.task == task; });
}

void Scheduler::setup(void)
{
    _time_mark = _ticker.mark();
}

bool Scheduler::add(Fptr task, unsigned long t2g)
{
    if (_tasks.end() != find(task)) {
        return false;
    }

    _tasks.push_back({t2g, task});

    return true;
}

bool Scheduler::drop(Fptr task)
{
    std::list<Task>::iterator it = find(task);

    if (_tasks.end() == find(task)) {
        return false;
    }

    _tasks.erase(it);

    return true;
}

bool Scheduler::supress(Fptr task)
{
    std::list<Task>::iterator it = find(task);

    if (_tasks.end() == find(task)) {
        return false;
    }

    it->t2g = -1;

    return true;
}

bool Scheduler::force(Fptr task)
{
    std::list<Task>::iterator it = find(task);

    if (_tasks.end() == find(task)) {
        return false;
    }

    it->t2g = 0;

    return true;
}

unsigned long Scheduler::run(void)
{
    const unsigned long mark = _ticker.mark();
    const unsigned long dt = mark - _time_mark;
    _time_mark = mark;

    for (std::list<Task>::iterator it = _tasks.begin(); it != _tasks.end(); ++it) {
        it->t2g -= it->t2g < dt ? it->t2g : dt;
        it->t2g = it->t2g ? it->t2g : it->task(dt);
    }

    unsigned long next = -1;

    for (std::list<Task>::iterator it = _tasks.begin(); it != _tasks.end(); ++it) {
        next = next < it->t2g ? next : it->t2g;
    }

    return next;
}
