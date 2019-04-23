/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		  	*
 *	   - Umberto Case		   	*
 *	   - Matteo Celani		   	*/
 
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "pcb.h"
#include "initArea.h"
#include "utils.h"

void checkEmptyProc(void);

void scheduler();

HIDDEN inline void priorityAging(void);
HIDDEN inline void restorePriority(pcb_t *pcb);

extern void log_process_order(u32 n);

#endif 
