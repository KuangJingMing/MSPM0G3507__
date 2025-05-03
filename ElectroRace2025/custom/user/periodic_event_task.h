#ifndef PERIODIC_EVENT_TASK_H
#define PERIODIC_EVENT_TASK_H

#include "FreeRTOS.h"
#include "task.h"

// Declare the periodic event task function
void PeriodicEventTask(void *pvParameters);

void create_periodic_event_task(void);


#endif // PERIODIC_EVENT_TASK_H
