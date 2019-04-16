#include "pcb.h"
#include "const.h"
#include "handler.h"
#include "interrupt.h"
#include "listx.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "p1.5test_rikaya_v0.c"

HIDDEN inline void initSYS(u32 status);
HIDDEN inline void initPGMTRP(u32 status);
HIDDEN inline void initINT(u32 status);
HIDDEN inline void initTLB(u32 status);
void initNEWAREA(void);
extern void test();
