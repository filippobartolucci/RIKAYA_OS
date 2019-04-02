#include "types_rikaya.h"
#include "const.h"
#include "pcb.h"


void priorityAging() {
	if (ready_queue) {
    		/* PCB temporaneo che uso per scorrere la ready_queue */
		pcb_t *tmp = readyQueue;
		/* Scorro tra processi che già hanno priorità massima */
		while (tmp->p_priority == MAXPRIO && tmp->p_next != NULL) tmp = tmp->p_next;
		/* Scorro tra i processi che non hanno priorità massima e la aumento */
		while (tmp->p_next != NULL) {
			tmp->p_priority++;
			tmp = tmp->p_next;
		}
	}

}

void deadlockDetected() {
	if (!ready_queue) {
		/* se process count = 0 invoco HALT ROM */
		if (process_count == 0) 
			HALT();
    		/* se process count > 0 e soft block count = 0 deadlock */
		else if (process_count > 0 && soft_block_count == 0)
			PANIC();
    		/* se process count e soft block count > 0 invoco un istruzione WAIT*/
		else if (process_count > 0 && soft_block_count > 0)
			WAIT();
	}
	else
	
		
}

