/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/

#ifndef _INITAREA_H
#define _INITAREA_H

#include "handler.h"
#include "listx.h"
#include "types_rikaya.h"


/* Lista dei processi ready */
struct list_head ready_queue;
/* Puntatore al processo attivo */
pcb_t* current_process;
/* Contatore processi */
u32 process_count;
/* Contatore processi bloccati per I/O */
u32 soft_block_count;

/* Puntatori alle NEW AREA della ROM */
state_t *sysbk_newarea;
state_t *program_trap_newarea;
state_t *interrupt_newarea;
state_t *tlbmgt_newarea;

/* Puntatori alle OLD AREA della ROM */
state_t *sysbk_oldarea;
state_t *program_trap_oldarea;
state_t *interrupt_oldarea;
state_t *tlbmgt_oldarea;

HIDDEN inline void initSYSBK(u32 status);
HIDDEN inline void initPGMTRP(u32 status);
HIDDEN inline void initINT(u32 status);
HIDDEN inline void initTLB(u32 status);

void initAREA(void);
void initNEWAREA(void);

#endif
