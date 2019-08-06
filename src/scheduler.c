/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/
 
#include "listx.h"
#include "scheduler.h"



void scheduler(void) {
	
    /* Ultimo processo in esecuzione */
    pcb_t *old = outProcQ(&ready_queue, current_process);
	
    /* Se old != NULL */
	if (old){
        /* Ripristino la sua priorità */
		restorePriority(old);
        /* Salvo lo stato dell'esecuzione prima dell'eccezione */
		memcpy(&old->p_s,interrupt_oldarea, sizeof(state_t));
        
        current->p_kernelt_total += TOD_LO - current->p_kernelt_start;
        current->p_kernelt_start = 0;
        
        /* Reinserisco il processi nella ready_queue */
        insertProcQ(&ready_queue,current_process);
 	}     
	
    /* Estraggo il processo con priorità più alta dalla ready_queue */
	current_process = headProcQ(&ready_queue);  
    
    /* Controllo se ci sono ancora processi da eseguire */
    checkEmptyProcQ();
    
    /* Aumento la priorità dei processi che sono nella ready_queue */
    priorityAging();
    /* Imposto il PLT */
    setTIMER(TIMESLICE * TIME_SCALE);
    /* Aggiorno il tempo */
    next->p_usert_start = TOD_LO;
    /* Carico lo stato del processo corrente */
    LDST(&current_process->p_s);
}

/* Funzione ausiliaria per verificare se il processo corrente è vuoto */
HIDDEN inline void checkEmptyProcQ(void){
    if(current_process == NULL && process_count == 0)
        HALT();
}

/* Funzione che si occupa del meccanismo di aging delle priorità dei PCB nella ready queue */
HIDDEN inline void priorityAging(void) {
	struct list_head *iter;
	list_for_each(iter, &ready_queue){
		pcb_t *tmp = container_of(iter, pcb_t, p_next);
		if (tmp->priority < MAXPRIO) tmp->priority++;
		}
}

/* Funzione che si occupa di ripristinare la priorità originale di un PCB */
HIDDEN inline void restorePriority(pcb_t *pcb){
	pcb->priority = pcb->original_priority; 	
}

