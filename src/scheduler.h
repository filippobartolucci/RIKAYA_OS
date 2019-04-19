#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "pcb.h"
#include "initArea.h"

void scheduler();

inline void priorityAging(void);
inline void restorePriority(pcb_t *pcb);

#endif 
