#include "handler.h"

/* Gestione SYSCALL/BREAKPOINT */
void sysbk_handler(void){
    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = sysbk_oldarea;
    
    /* Registro nel quale è salvata la SYSCALL chiamata */
    u32 syscall_number = old_state->reg_a0;
        
    /* Parametri della SYSCALL */
    u32 *arg1 =  &old_state->reg_a1;
    u32 *arg2 =  &old_state->reg_a2;
    u32 *arg3 =  &old_state->reg_a3;
    
        
    
    /* Gestione dei BREAKPOINT da implementare nella PHASE2 
     * Ignorati per PHASE1.5                                
    */
        
    switch (syscall_number){
        /* Eseguo la SYSCALL richiesta */
        case TERMINATEPROCESS:
            // SYSCALL TERMINATE PROCESS
            break;
        default:
            /* Errore numero SYSCALL inesistente */
            PANIC();
    }
    scheduler();
    
    /* Gestione dei BREAKPOINT da implementare nella PHASE2 */
}

/* Gestione INTERRUPT */
void int_handler(void){
    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = interrupt_oldarea;
    /* Causa dell'interrupt */
    u32 cause = old_state->cause;
    
    /* Cerco il device che ha sollevato l'interrupt */
    
    /* I bit da 8 a 15 indicano quale linea interrupt sia attiva
     * Utilizziamo uno shift per eliminare i bit meno significativi che non ci servono
    */  
    cause = cause >> 8;
    u32 line;
    
    /* Ricerca dell' interrupt */
    if (cause == (cause | 0x1))          /* 00000001 */	  
		line = 0;

    else if (cause == (cause | 0x2))     /* 00000010 */  
		line = 1;
		/* Processor Local Timer */
    else if (cause == (cause | 0x4))     /* 00000100 */	
		 line = 2;
		/* Interval Timer */
    else if (cause == (cause | 0x8))     /* 00001000 */
		line = 3;
		/* Disk */
    else if (cause == (cause | 0x10))	 /* 00010000 */	
		line = 4;
		/* Tape */
    else if (cause == (cause | 0x20))    /* 00100000 */
		line = 5;
		
    else if (cause == (cause | 0x40))    /* 01000000 */
		line = 6;
		/* Printer */
    else line =7; 
	/* Terminal */                          
    
	
    switch(line){
    case INT_PLT:
    	    scheduler();
	    break;
    deault:
            break;
    }
	
  
    /* Gestione degli INTERRUPT dei device da implementare nella PHASE2 */  
}

/* Gestione TLB */
void tlb_handler(void){
    /* Da implementare nella PHASE2 */
}

/* Gestione PGMTRP */
void pgmtrp_handler(void){
    /* Da implementare nella PHASE2 */
}


/* SYSTEMCALL */
/* HIDDEN perché devono essere accessibili solo da sysbk_handler */


/* SYSCALL3
 * Quando invocata termina il processo corrente
 * e tutta la sua progenie, rimuovendoli dalla
 * Ready Queue.
*/
HIDDEN void terminateProcess(pcb_t *p){  
    while(!list_empty(&p->p_child))
        freePcb(removeChild(&p->p_child));
    outChild(p);
    freePcb(p);
    process_count--;
    scheduler();
}
