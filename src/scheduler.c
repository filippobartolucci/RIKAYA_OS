#include "listx.h"
#include "scheduler.h"

void scheduler(void) 
{
	termprint("inizio scheduler\n", 0);
	/*gestione dei deadlock poi dei processi*/
	if (current_process) 
	{
		/* Caricamento del Timer */
		setTIMER(TIMESLICE);
		
		
		
		/* Carica i processi nell'ordine in cui devono essere eseguiti */
		LDST(&(current_process->p_s));
	}
	else 
	{
		/* Caso Ready Queue vuota*/
		if (emptyProcQ(&ready_queue))
		    {
			/* Non ci sono altri processi*/
			if (process_count == 0) 
				HALT();
    			/* Rilevazione di Deadlock*/
			if (process_count > 0 && soft_block_count == 0)
				PANIC();
    			/* Un processo e bloccato */
			if (process_count > 0 && soft_block_count > 0)
				WAIT();					//si dovrebbero gestire anche gli interupts (abilitare gli interupts)
		    }
		    PANIC();
	}
	
	/*Verifica che i processi vengano alternati correttamente*/
	log_process_order(current_process->original_priority) ;
		
}

// restorePriority e priorityAgin vanno chiamate dopo aver estratto il processo (con priorità massima) dalla readyqueue

/* Funzione che si occupa del meccanismo di aging delle priorità dei PCB nella ready queue */
HIDDEN inline void priorityAging(void) {
	//if (!list_empty(ready_queue)) {
    		/* PCB temporaneo che uso per scorrere la ready_queue */
	//	pcb_t *tmp = readyQueue;
		/* Scorro tra processi che già hanno priorità massima */
	//	while (tmp->p_priority == MAXPRIO && tmp->p_next != NULL) tmp = tmp->p_next;
		/* Scorro tra i processi che non hanno priorità massima e la aumento */
	//	while (tmp->p_next != NULL) {
	//		tmp->p_priority++;
	//		tmp = tmp->p_next;
	//	}
	//}

}

/* Funzione che si occupa di ripristinare la priorità originale di un PCB */
HIDDEN inline void restorePriority(pcb_t *pcb){
	pcb->priority = pcb->original_priority; 	
}
//gestire un solo inturupt quello del tempo
//l'unica cosa che esegue lo scheduler e il passaggio da un processo ad un altro
