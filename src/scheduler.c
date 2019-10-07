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
      old->priority = old->original_priority;
      /* Salvo lo stato dell'esecuzione prima dell'eccezione */
      memcpy(&old->p_s,interrupt_oldarea, sizeof(state_t));
      /* Reinserisco il processi nella ready_queue */
      insertProcQ(&ready_queue,current_process);
    }
    /* Controllo se ci sono ancora processi da eseguire */
    if(current_process == NULL && list_empty(&ready_queue)){
      setSTATUS(getSTATUS()|1UL);
      WAIT();
    }
    /* Estraggo il processo con priorità più alta dalla ready_queue */
    current_process = headProcQ(&ready_queue);
    /* Aumento la priorità dei processi che sono nella ready_queue */
    priorityAging();
    /* Aggiorno il tempo */
    current_process->user_time_start = TOD_LO;
    /* Imposto il PLT */
    setTIMER(TIMESLICE * TIME_SCALE);
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
