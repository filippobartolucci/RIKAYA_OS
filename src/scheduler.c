#include "listx.h"
#include "scheduler.h"

void scheduler(void) 
{
	termprint("inizio scheduler\n", 0);
	/*gestione dei deadlock poi dei processi*/
	if (current_process == NULL){
		if(process_count == 0)
			HALT();
		current_process = removeProcQ(&ready_queue);
		restorePriority(current_process);
		priorityAging();
		log_process_order(current_process->original_priority);
		setTIMER(TIMESLICE);
		LDST(current_process->p_s);
	}
		
}

// restorePriority e priorityAgin vanno chiamate dopo aver estratto il processo (con priorità massima) dalla readyqueue

/* Funzione che si occupa del meccanismo di aging delle priorità dei PCB nella ready queue */
HIDDEN inline void priorityAging(void) {
	struct list_head *iter;
	list_for_each(iter, &ready_queue){
		pcb_t *tmp = container_of(iter, pcb_t, p_next);
		if (tmp->priority<20) tmp->priority++;
		}
	
		
	
	
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
	/*
	pcb_t* tmp;
	tmp = headProcQ(&ready_queue);
	int i = 0;
	list_for_each_entry(tmp, &tmp->p_next , p_next){
		if(tmp->priority < MAXPRIO)
			tmp->priority++;
	}
	*/

}

/* Funzione che si occupa di ripristinare la priorità originale di un PCB */
HIDDEN inline void restorePriority(pcb_t *pcb){
	pcb->priority = pcb->original_priority; 	
}
//gestire un solo inturupt quello del tempo
//l'unica cosa che esegue lo scheduler e il passaggio da un processo ad un altro
