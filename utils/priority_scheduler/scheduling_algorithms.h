/** \file scheduling_algorithms.h
 * This file exports the datastructres used to select the algorithms that will govern the scheduler.
 */

#ifndef SCHEDULING_ALGORITHMS_H_INCLUDED
#define SCHEDULING_ALGORITHMS_H_INCLUDED

#include "priority_scheduler.h"

/** \brief Selects the queue to be used for scheduling out the jobs
 * \param[in] sched The schedule which needs to pick a queue.
 * \returns The queue index or ::SCHEDULE_FAIL
 * The queue is selected according to the schduling policy defined in the scheduler
 */
int select_queue(priority_scheduler* sched);

#endif // SCHEDULING_ALGORITHMS_H_INCLUDED