#include "pcb.h"
#include "const.h"
#include "handler.h"
#include "interrupt.h"
#include "listx.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "p1.5test_rikaya_v0.c"

/* Lista dei processi ready */
extern INIT_LIST(ready_queue);
/* Puntatore al processo atti,vo */
extern pcb_t* current_process
/* Contatore processi */
extern u32 process_count;
/* Contatore processi bloccati per I/O */
extern u32 soft_block_count;

HIDDEN inline void initSYS(u32 status);
HIDDEN inline void initPGMTRP(u32 status);
HIDDEN inline void initINT(u32 status);
HIDDEN inline void initTLB(u32 status);
void initNEWAREA(void);

