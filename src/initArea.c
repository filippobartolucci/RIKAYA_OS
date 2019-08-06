/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/
 
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
    sysbk_newarea->pc_epc = (memaddr)sysbk_handler;  
    sysbk_newarea->reg_t9 = (memaddr)sysbk_handler;
}

HIDDEN inline void initPGMTRP(u32 status){  
    program_trap_newarea->status = status;
    program_trap_newarea->reg_sp = RAMTOP;
    program_trap_newarea->pc_epc = (memaddr)pgmtrp_handler;
    program_trap_newarea->reg_t9 = (memaddr)pgmtrp_handler;
}
    
HIDDEN inline void initINT(u32 status){  
    interrupt_newarea->status = status;
    interrupt_newarea->reg_sp = RAMTOP;
    /* Indirizzo della funzione che gestisce gli INTERRUPT */
    interrupt_newarea->pc_epc = (memaddr)int_handler;
    interrupt_newarea->reg_t9 = (memaddr)int_handler;
}

HIDDEN inline void initTLB(u32 status){  
    tlbmgt_newarea->status = status;
    tlbmgt_newarea->reg_sp = RAMTOP;
    tlbmgt_newarea->pc_epc = (memaddr)tlb_handler;
    tlbmgt_newarea->reg_t9 = (memaddr)tlb_handler;
}



/* Funzione che si occupa dell'inizializzazione delle NEWAREA della ROM.
 * Viene essere settato il campo status in maniera tale da:
 *  - Mascherare gli interrupt
 *  - Disabilitare la virtual memory
 *  - Abilita il processor local timer 
 *  - Essere in kernel mode
*/
void initAREA(void){  
    
    u32 s = 0;
    /* Imposto il valore dello stato tramite la Macro STATUS_NEW_AREA definita in const.h */ 
    s |= STATUS_NEW_AREA ;
    
    /* Metto a 0 tutte le newarea della roma */
    memset(sysbk_newarea, 0, sizeof(state_t));
    memset(program_trap_newarea, 0, sizeof(state_t));
    memset(interrupt_newarea, 0, sizeof(state_t));
    memset(tlbmgt_newarea, 0, sizeof(state_t));

    /* Inizializzazione delle 4 aree */
    initSYSBK(s);
    initPGMTRP(s);
    initINT(s);
    initTLB(s);
}


