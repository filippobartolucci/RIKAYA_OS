/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/

#ifndef _UTILS_H
#define _UTILS_H

#include "pcb.h"
#include "const.h"
#include "listx.h"
#include "types_rikaya.h"

/* Funzioni ausiliarie */
void *memset(void *s, u32 c, int n);
void memcpy(void *dest, void *src, u32 n);

/* Funzioni di sistema */
extern void HALT();
extern void PANIC();
extern void setTIMER(u32 n);
extern void LDST(state_t *s);
extern void SYSCALL(int *a0, int *a1, int *a3, int *a4);
extern int getSTATUS();
extern int setSTATUS();




#endif
