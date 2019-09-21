/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/

#ifndef _HANDLER_H
#define _HANDLER_H

#include "listx.h"
#include "utils.h"
#include "initArea.h"
#include "scheduler.h"

/* Funzioni per la gestione delle eccezioni */
void sysbk_handler(void);
void int_handler(void);
void tlb_handler(void);
void pgmtrp_handler(void);

int semd_keys[8][7];
int waitc_sem;
pcb_t *waiting_pcbs[8][7];
/* Funzioni ausiliarie */
HIDDEN u32 whichConst(u32 line);
HIDDEN inline int whichDevice(u32* bitmap);

/* SYSCALL */
HIDDEN void getCpuTime(unsigned int* user, unsigned int* kernel, unsigned int* wallclock);
HIDDEN int createProcess(state_t* statep, int priority, void** cpid);
HIDDEN int terminateProcess(void ** pid);
HIDDEN pcb_t* Verhogen(int* semaddr);
HIDDEN void Passeren(int *semaddr);
HIDDEN void Wait_Clock(void);
HIDDEN int Do_IO(u32 command, u32 *reg, int transm);
HIDDEN void Set_Tutor();
HIDDEN int Spec_Passup(int type, state_t *old, state_t *new);
HIDDEN void Get_pid_ppid(void ** pid, void ** ppid);

#endif
