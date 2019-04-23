/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		  	*
 *	   - Umberto Case		   	*
 *	   - Matteo Celani		   	*/
 
#include "listx.h"
#include "scheduler.h"

/* Funzione ausiliaria per verificare se il processo corrente è vuoto */
void checkEmptyProc(void){
	if(current_process == NULL && process_count == 0)
		HALT();
}

void scheduler(void) {
	
    //checkEmptyProc();
    /* Ultimo processo in esecuzione */
    pcb_t *old = outProcQ(&ready_queue, current_process);
	
    /* Se old != NULL */
	if (old){
        /* Ripristino la sua priorità */
		restorePriority(old);
        /* Salvo lo stato dell'esecuzione prima dell'eccezione */
		memcpy(&old->p_s,interrupt_oldarea, sizeof(state_t));
        /* Reinserisco il processi nella ready_queue */
		insertProcQ(&ready_queue,current_process);
 	}     
	
    /* Estraggo il processo con priorità più alta dalla ready_queue */
	current_process = headProcQ(&ready_queue);  
    
    /* Controllo se ci sono ancora processi da eseguire */
    if (current_process == NULL)
        if(process_count == 0)
            HALT();
    
    /* Imposto il PLT */
	setTIMER(TIMESLICE * TIME_SCALE);
    /* Aumento la priorità dei processi che sono nella ready_queue */
    priorityAging();
	log_process_order(current_process->original_priority);
    /* Carico lo stato del processo corrente */
	LDST(&current_process->p_s);
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

