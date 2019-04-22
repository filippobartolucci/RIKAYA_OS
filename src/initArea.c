#include "pcb.h"
#include "const.h"
#include "handler.h"
#include "listx.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "initArea.h"

/* Funzioni per l'inizializzazione delle NEW AREA */
HIDDEN inline void initSYSBK(u32 status){
    sysbk_newarea->status = status;
    sysbk_newarea->reg_sp = RAMTOP;
    /* Indirizzo della funzione che gestisce SYSCALL e BREAKPOINT */
    sysbk_newarea->pc_epc = sysbk_handler;  
    //sysbk_newarea->reg_t9 = sysbk_handler;
}

HIDDEN inline void initPGMTRP(u32 status){  
    program_trap_newarea->status = status;
    program_trap_newarea->reg_sp = RAMTOP;
    program_trap_newarea->pc_epc = pgmtrp_handler;
    program_trap_newarea->reg_t9 = pgmtrp_handler;
}
    
HIDDEN inline void initINT(u32 status){  
    interrupt_newarea->status = status;
    interrupt_newarea->reg_sp = RAMTOP;
    /* Indirizzo della funzione che gestisce gli INTERRUPT */
    interrupt_newarea->pc_epc = int_handler;
    //interrupt_newarea->reg_t9 = int_handler;
}

HIDDEN inline void initTLB(u32 status){  
    tblmgt_newarea->status = status;
    tblmgt_newarea->reg_sp = RAMTOP;
    tblmgt_newarea->pc_epc = tlb_handler;
    tblmgt_newarea->reg_t9 = tlb_handler;
}



/* Funzione che si occupa dell'inizializzazione delle NEWAREA della ROM.
 * Viene essere settato il campo status in maniera tale da:
 *  - Mascherare gli interrupt
 *  - Disabilitare la virtual memory
 *  - Abilita il processor local timer 
 *  - Essere in kernel mode
*/
void initAREA(void){  
    
     /* Nello status:
      *  Maschero interrupt
      *  Setto Virtual Memory OFF
      *  attivo il Processor Local Timer
      *  abilito il kernel-mode
      */
    u32 s = 0; 
    s |= 1 << 28| 1 << 27 | 1 << 2;
    
    /* Metto a 0 tutte le newarea della roma */
    memset(sysbk_newarea, 0, sizeof(state_t));
    memset(program_trap_newarea, 0, sizeof(state_t));
    memset(interrupt_newarea, 0, sizeof(state_t));
    memset(tblmgt_newarea, 0, sizeof(state_t));

    /* Inizializzazione delle 4 aree */
    initSYSBK(s);
    initPGMTRP(s);
    initINT(s);
    initTLB(s);
}

