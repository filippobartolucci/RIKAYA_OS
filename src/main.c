#include "pcb.h"
#include "const.h"
#include "handler.h"
#include "listx.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "initArea.h"
#include "utils.h"

extern void test1();
extern void test2();
extern void test3();



/* Lista dei processi ready */
LIST_HEAD(ready_queue);
/* Puntatore al processo attivo */
pcb_t* current_process = NULL;
/* Contatore processi */
u32 process_count = 0;
/* Contatore processi bloccati per I/O */
u32 soft_block_count = 0;

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


/* Funzione per inizializzare un pcb_t settando:
     * Interrupt ON;
     * Virtual Memory OFF;
     * Processor Local Timer ON;
     * $SP a RAMSIZE-FRAMESIZE*n;
     * Priorità uguale ad n
     * pc_epc = (memaddr) testn;
*/
void setProcess(memaddr proc, int n){
	pcb_t *tmp = allocPcb();
	tmp->p_s.pc_epc = proc;
	tmp->p_s.reg_t9 = proc;
	tmp->priority = n;
	tmp->original_priority = n;
	tmp->p_s.reg_sp = RAMTOP - FRAME_SIZE * n;
	tmp->p_s.status = 1800FF04;    //0 | 1<<27 | (0xFF00-0x8000);
	
	current_process++;
	insertProcQ(&ready_queue, tmp);
}

int main(void){

    termprint("initArea()\n", 0);
    initAREA();
    termprint("initPcbs()\n", 0);
    initPcbs();

   

    termprint("setProcess 1,2,3 \n", 0);
    
    setProcess(test1, 1);
    setProcess(test2, 2);
    setProcess(test3, 3);

    termprint("Scheduler()\n",0);
	
    /* Passo il controllo allo scheduler */
    scheduler();

    return 0;
}

