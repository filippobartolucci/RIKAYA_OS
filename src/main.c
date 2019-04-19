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
INIT_LIST(ready_queue);
/* Puntatore al processo attivo */
pcb_t* current_process = NULL;
/* Contatore processi */
u32 process_count = 0;
/* Contatore processi bloccati per I/O */
u32 soft_block_count = 0;

/* Puntatori alle NEW AREA della ROM */
HIDDEN state_t *sysbk_newarea = SYSBK_NEWAREA;
HIDDEN state_t *program_trap_newarea = PRGTRP_NEWAREA;
HIDDEN state_t *interrupt_newarea = INT_NEWAREA;
HIDDEN state_t *tblmgt_newarea = TLB_NEWAREA;

/* Puntatori alle OLD AREA della ROM */
HIDDEN state_t *sysbk_oldarea = SYSBK_OLDAREA;
HIDDEN state_t *program_trap_oldarea = PRGTRP_OLDAREA;
HIDDEN state_t *interrupt_oldarea = INT_OLDAREA;
HIDDEN state_t *tblmgt_oldarea = TBL_OLDAREA;


/* Funzione per inizializzare un pcb_t settando:
     * Interrupt ON;
     * Virtual Memory OFF;
     * Processor Local Timer ON;
     * $SP a RAMSIZE-FRAMESIZE*n;
     * Priorità uguale ad n
     * pc_epc = (memaddr) testn;
*/
void setProcess(pcb_t* process, int n){
	process->p_s.status |= (STATUS_IEc);
	process->p_s.status |= ~(STATUS_KUc);
	process->p_s.status |= ~(STATUS_VMc);
	process->p_s.status |= (STATUS_TE);
	process->priority = n;
	process->p_s.reg_sp = (RAMTOP) - FRAME_SIZE * n;
	process->p_s.reg_t9 = (RAMTOP) - FRAME_SIZE * n;
	process->p_s.pc_epc = (memaddr)test;
}

int main(void){
    
    initAREA();
    initPcbs();
    
    /* Instanzio il processo corrente */
    current_process = allocPCB();
    process_count++;

    /* Setto i dovuti campi del processo corrente */
    int n = 1;    
    /*Questo andrebbe fatto per tutti e tre i test, scorrendo la lista dei processi
     * ma non so se basta passare current_process->p_sib a setProcess
     * dato che sarebbe un tipo list_head... dovremmo passare il pcb
     */
    setProcess(current_process, n);
    list_add_tail(&(current_process->p_next), ready_queue);


    /* Passo il controllo allo scheduler */
    scheduler();

    return 0;
}
