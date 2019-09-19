
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

/* Traced Regions */
u32 debug = 0;


/* Funzioni di test per PHASE2 */
extern void test();

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
state_t *tlbmgt_newarea = (state_t *)TLB_NEWAREA;

/* Puntatori alle OLD AREA della ROM */
state_t *sysbk_oldarea = (state_t *)SYSBK_OLDAREA;
state_t *program_trap_oldarea = (state_t *)PGMTRAP_OLDAREA;
state_t *interrupt_oldarea = (state_t *)INT_OLDAREA;
state_t *tlbmgt_oldarea = (state_t *)TLB_OLDAREA;

int setBit (int shift, unsigned int *reg, int value){
        unsigned int tmp = 1;
        tmp = tmp << shift;
        if (value == 1) *reg = *reg | tmp;
        else if (value == 0){
                tmp = ~tmp;
                *reg = *reg & tmp;
        }
        return 0;
}

void initProc(){
		pcb_t *test=allocPcb;
		setBit(0,&(test->p_s.status),1);
		setBit(24,&(test->p_s.status),0);
		setBit(27,&(test->p_s.status),1);
		setBit(1,&(test->p_s.status),1);
		test->p_s.status|=(255<<8);
		test->p_s.status|=(1UL<<0);
		test->p_s.status|=(1UL<<2); 
		test->p_s.reg_sp = RAMTOP-FRAME_SIZE;
		test->priority = 1;
		test->original_priority= 1;
		test->p_s.pc_epc = (unsigned int) test;
		test->p_s.reg_t9 = test_pcb->p_s.pc_epc;
		insertProcQ(&ready_queue, tmp);
}

int main(void){
    /* Inizializzazione del sistema */
    initAREA();
    initPcbs();
    initASL();
    /* Setto Interval Timer */
    *((u32 *)INT_TIMER) = (u32)PSEUDO_CLOCK_TICK;
		/* Inizializzo semafori */
		memset(&semd_keys,1,(sizeof(int))*7*8);
		waitc_sem = 0;

		/* Imposto il primo processo */
    initProc();
    /* Passo il controllo allo scheduler */
    scheduler();
    /* L'esecuzione non deve mai arrivare qui */
    PANIC();
    return 0;
}



void setProcess(u32 proc,int n,int m){
    /* Prendo un PCB dalla lista dei PCB liberi */
	pcb_t *tmp = allocPcb();
    /* Imposto il PROGRAM COUNTER del processo */
	tmp->p_s.pc_epc = tmp->p_s.reg_t9 = (u32)proc;
    /* Imposto la priorità */
	tmp->priority = tmp->original_priority = n;
    /* Imposto lo STACK POINTER */
	tmp->p_s.reg_sp = RAMTOP - FRAME_SIZE*m;
    /* Imposto lo STATUS del process */
	tmp->p_s.status =0x0800ff07<
	//0x0800ff07
ß
  /* Aumento il contatore dei processi */
	process_count++;
  /* Inserisco il PCB nella lista dei processi in stato ready */
	insertProcQ(&ready_queue, tmp);
}
