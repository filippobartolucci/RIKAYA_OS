#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "pcb.h"
#include "initArea.h"

void scheduler();

HIDDEN inline void priorityAging(void);
HIDDEN inline void restorePriority(pcb_t *pcb);

#endif 
