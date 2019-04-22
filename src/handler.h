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

HIDDEN void terminateProcess();
#endif
