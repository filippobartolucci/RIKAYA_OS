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
void setProcess(pcb_t* process, int n){
	process->p_s.status = 0 | 1<<27 | (0xFF00-0x8000);
	process->priority = n;
	process->p_s.reg_sp = (RAMTOP) - FRAME_SIZE * n;
}

int main(void){

    termprint("initArea()\n", 0);
    initAREA();
    termprint("initPcbs()\n", 0);
    initPcbs();

    /* Instanzio il processo corrente */
    current_process = allocPcb();
    process_count++;  
  
    setProcess(current_process,1);
    current_process->p_s.pc_epc = (memaddr)test1;
    current_process->p_s.reg_t9 = (memaddr)test1;
    insertProcQ(&ready_queue, current_process);


    /* Instanzio il processo corrente */
    current_process = allocPcb();
    process_count++;  
  
    setProcess(current_process,2);
    current_process->p_s.pc_epc = (memaddr)test2;
    current_process->p_s.reg_t9 = (memaddr)test2;
    insertProcQ(&ready_queue, current_process);



    /* Instanzio il processo corrente */
    current_process = allocPcb();
    process_count++;  
  
    setProcess(current_process,3);
    current_process->p_s.pc_epc = (memaddr)test3;
    current_process->p_s.reg_t9 = (memaddr)test3;
    insertProcQ(&ready_queue, current_process);


    current_process=NULL;


    termprint("setProcess(current_process)\n", 0);

    termprint("scheduler()\n",0);
    /* Passo il controllo allo scheduler */
    scheduler();

    return 0;
}

