/*						*
 *		 PHASE2 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/
 
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "pcb.h"
#include "initArea.h"
#include "utils.h"

void scheduler();

HIDDEN inline void checkEmptyProc(void);
HIDDEN inline void priorityAging(void);
HIDDEN inline void restorePriority(pcb_t *pcb);

#endif 
