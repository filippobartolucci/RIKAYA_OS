
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
#include "utils.h"
#include "asl.h"

/* Funzioni di test per PHASE2 */
extern void test();

void system(){
	state_t test_s;
	
	memset(&test_s, 0, sizeof(test_s));
	test_s.pc_epc = (memaddr)test;
	test_s.reg_sp = RAMTOP - FRAME_SIZE * 2;  /* First stack is for the system process */
	test_s.status = (1<<2|0xFF00|1<<27);
	SYSCALL(SETTUTOR, 0, 0, 0);
	SYSCALL(CREATEPROCESS, (u32)&test_s, 1, 0);

	while(1); 
}

/* Lista dei processi ready */
LIST_HEAD(ready_queue);
/* Puntatore al processo attivo */
pcb_t* current_process = NULL;
/* Contatore processi */
u32 process_count = 0;

/* Puntatori alle NEW AREA della ROM */
state_t *sysbk_newarea = (state_t *)SYSBK_NEWAREA;
state_t *program_trap_newarea = (state_t *)PGMTRAP_NEWAREA;
state_t *interrupt_newarea = (state_t *)INT_NEWAREA;
state_t *tblmgt_newarea = (state_t *)TLB_NEWAREA;

/* Puntatori alle OLD AREA della ROM */
state_t *sysbk_oldarea = (state_t *)SYSBK_OLDAREA;
state_t *program_trap_oldarea = (state_t *)PGMTRAP_OLDAREA;
state_t *interrupt_oldarea = (state_t *)INT_OLDAREA;
state_t *tblmgt_oldarea = (state_t *)TLB_OLDAREA;


void setProcess(u32 *proc){
    /* Prendo un PCB dalla lista dei PCB liberi */
	pcb_t *tmp = allocPcb();
    /* Imposto il PROGRAM COUNTER del processo */
	tmp->p_s.pc_epc = proc;
	tmp->p_s.reg_t9 = proc;
    /* Imposto la priorità */
	tmp->priority = tmp->original_priority = 1;
    /* Imposto lo STACK POINTER */
	tmp->p_s.reg_sp = RAMTOP - FRAME_SIZE * 1;
    /* Imposto lo STATUS del process */
	tmp->p_s.status = (1<<2|0xFF00|1<<27);
    /* Setto il tempo */
    tmp->p_wallclock_start = TOD_LO;
    /* Aumento il contatore dei processi */
	process_count++;
    /* Inserisco il PCB nella lista dei processi in stato ready */
	insertProcQ(&ready_queue, tmp);
}

int main(void){
    /* Inizializzazione del sistema */
    initAREA();
    initPcbs();
    initASL();
    /* Setto Interval Timer */
    *((u32 *)INT_TIMER) = (u32)PSEUDO_CLOCK_TICK;

    setProcess((u32*)test);
    setProcess((u32*)system);
    /* Passo il controllo allo scheduler */
    scheduler();

    /* L'esecuzione non deve mai arrivare qui */
    PANIC();
    return 0;
}
