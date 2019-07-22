/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/
 
#include "handler.h"

/* Gestione SYSCALL/BREAKPOINT */
void sysbk_handler(void){

    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = sysbk_oldarea;
    
    /* Registro nel quale è salvata la SYSCALL chiamata */
    u32 syscall_number = old_state->reg_a0;
        
    /* Parametri della SYSCALL 
     * Inutilizzati in PHASE1.5
     *  u32 *arg1 =  &old_state->reg_a1;
     *  u32 *arg2 =  &old_state->reg_a2;
     *  u32 *arg3 =  &old_state->reg_a3;
    */
    
    switch (syscall_number){
        /* SYS 1*/
        case GETCPUTIME:
            break:
        
        /* SYS 2*/
        case CREAPROCESS:
            
            break:
        
        /* SYS 3*/
        case TERMINATEPROCESS:
            terminateProcess();
            break;
        
        /* SYS 4*/
        case PASSEREN:
            
            break;
        
        /* SYS 5*/
        case VERHOGEN:
            
            break;
        
        /* SYS 6*/
        case WAITCLOCK:
            
            break;
        
        /* SYS 7*/
        case WAITIO:
            
            break;
            
        /* SYS 8*/
        case SETTUTOR:
            
            break;
        
        /* SYS 9*/
        case SPECPASSUP:
            
            break;
        
        /* SYS 10*/
        case GETPID;
            
            break:
        
        default:
            PANIC();
    }
    
    old_state->pc_epc += WORD_SIZE;
    scheduler();
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

    /* Ricerca dell' interrupt 
     * Controlli fatti in ordine di priorità.
     * I bit meno significativi hanno priorità
     * maggiore. Il controllo confronta 
     * la causa dell'interrupt con una causa in 
     * cui il bit del device che si sta controllando è a 1.
    */

    if (cause == (cause | 0x1))          /* 00000001 */	  
        ;
        /* Interrupt Line 0 */
    
    else if (cause == (cause | 0x2))     /* 00000010 */  
		/* Processor Local Timer */
        scheduler();

    else if (cause == (cause | 0x4))     /* 00000100 */	
        ;
        /* Interrupt Line 2 */

    else if (cause == (cause | 0x8))     /* 00001000 */
		;
        /* Disk */

    else if (cause == (cause | 0x10))	 /* 00010000 */	
		;
        /* Tape */
		
    else if (cause == (cause | 0x20))    /* 00100000 */
	    ;
        /* Interrupt Line 5 */

    else if (cause == (cause | 0x40))    /* 01000000 */
		;
        /* Printer */
    
    else/* Terminal */ ;  

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
/* HIDDEN, accessibili solo da sysbk_handler */




/* SYSCALL3
 * Quando invocata termina il processo corrente
 * e tutta la sua progenie, rimuovendoli dalla
 * Ready Queue.
*/
HIDDEN void terminateProcess(){
	pcb_t *p, *child;
	struct list_head *iter;
	
	/* Associo il processo corrente ad un pcb_t interno alla funzione */
	p = outProcQ(&ready_queue, current_process);
	
	/* Itero la lista dei figli del processo e li rimuovo
	 * spostandoli nella lista dei pcb liberi */
        list_for_each(iter, &p->p_child){
		child = container_of(iter, pcb_t, p_sib);
		outProcQ(&ready_queue, child);
        	freePcb(removeChild(child));
	}
	
	/* Setto il processo corrente a NULL e libero tutto il resto 
	 * decremento il process_count e quando non ci sono più processi
	 * arriva a 0*/
	current_process = NULL;
	outChild(p);
    	freePcb(p);
    	process_count--;

	/* Il controllo ritorna allo scheduler */
    	scheduler();
}
