#include "listx.h"
#include "scheduler.h"

void scheduler(void) {
	pcb_t *old = outProcQ(&ready_queue, current_process);
	if (old){
		memcpy(interrupt_oldarea, &old->p_s, sizeof(state_t));
		restorePriority(old);
		insertProcQ(&ready_queue,current_process);
 	}     	       
	current_process = headProcQ(&ready_queue);
	priorityAging();
	setTIMER(TIMESLICE * TIMESCALE);
	log_process_order(current_process->original_priority);
	LDST(&current_process->p_s);
}

/* Funzione che si occupa del meccanismo di aging delle priorità dei PCB nella ready queue */
HIDDEN inline void priorityAging(void) {
	struct list_head *iter;
	list_for_each(iter, &ready_queue){
		pcb_t *tmp = container_of(iter, pcb_t, p_next);
		if (tmp->priority<20) tmp->priority++;
		}
}

/* Funzione che si occupa di ripristinare la priorità originale di un PCB */
HIDDEN inline void restorePriority(pcb_t *pcb){
	pcb->priority = pcb->original_priority; 	
}

