/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/
 
#include "handler.h"

/* Gestione SYSCALL/BREAKPOINT */
void sysbk_handler(void){

    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = sysbk_oldarea;
    
    /* Registro nel quale è salvata la SYSCALL chiamata */
    u32 syscall_number = old_state->reg_a0;
        
    /* Parametri della SYSCALL */ 
     u32 *arg1 =  &old_state->reg_a1;
     u32 *arg2 =  &old_state->reg_a2;
     u32 *arg3 =  &old_state->reg_a3;
        
    /* Gestione dei BREAKPOINT da implementare nella PHASE2 
     * Ignorati per PHASE1.5                                
    */

    int flag = 0;
        
    switch (syscall_number){
        /* Eseguo la SYSCALL richiesta */
		case GETCPUTIME:
	    	getCpuTime((u32*) arg1,(u32*) arg2,(u32*) arg3);
	    	break;
			
		case CREATEPROCESS:
	   		flag = createProcess((state_t*)arg1, (int)old_state->reg_a2, (void **)arg3);
	    	break;
			
		case VERHOGEN:
	    	verhogen((int*)arg1);
	    	break;
			
		case TERMINATEPROCESS:
            terminateProcess();
            break;
			

		case PASSEREN:
      		Passeren();
      		break;

  	  	case VERHOGEN:
    	 	Verhogen();
      		break;
		
		case WAITCLOCK:
      		Wait_Clock();
 		    break;

    	case WAITIO:
      		Do_IO();
      		break;

    	case SETTUTOR:
      		Set_Tutor();
      		break;

    	case SPECPASSUP:
     		ret = Spec_Passup();
      		break;

    	case GETPID:
      		Get_pid_ppid();
      		break;

        default:
            /* Errore numero SYSCALL inesistente */
            PANIC();
    }
	
    scheduler();  
}

/* Gestione INTERRUPT */
void int_handler(void){

    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = interrupt_oldarea;
    /* Causa dell'interrupt */
    u32 cause = old_state->cause;
   
	/* Struttura per il dispositivo */
    dtpreg_t *dev;	
	/* Struttura per il terminale */
	termreg_t *term;
	
    /* Cerco il dispositivo che ha sollevato l'interrupt */
    u32 line;
	
    /* I bit da 8 a 15 indicano quale linea interrupt sia attiva
     * Utilizziamo uno shift per eliminare i bit meno significativi che non ci servono
    */  

    cause = cause >> 8;
	
    /* Ricerca dell' interrupt 
     * Controlli fatti in ordine di priorità.
     * I bit meno significativi hanno priorità
     * maggiore. Il controllo confronta 
     * la causa dell'interrupt con una causa in 
     * cui il bit del dispositivo che si sta controllando è a 1.
    */

    if (cause == (cause | 0x1))          /* 00000001 */	  
        line = 0;
        /* Interrupt Line 0 */
    
    else if (cause == (cause | 0x2))     /* 00000010 */  
		/* Processor Local Timer */
        scheduler();

    else if (cause == (cause | 0x4))     /* 00000100 */	
        line = 2;
        /* Interrupt Line 2 */

    else if (cause == (cause | 0x8))     /* 00001000 */
		line = 3;
        /* Disk */

    else if (cause == (cause | 0x10))	 /* 00010000 */	
		line = 4;
        /* Tape */
		
    else if (cause == (cause | 0x20))    /* 00100000 */
		line = 5;
        /* Interrupt Line 5 */

    else if (cause == (cause | 0x40)){   /* 01000000 */
		/* Interrupt Line 6 */	
		line = 6;
		int dev_num = whichDevice((u32*)INT_BITMAP_PRINTER);
		
		/*
		Codice per i semafori e processi bloccati ai semafori
		*/
		
		/* Invio l'ACK al dispositivo trovato */
		dev = (dtpreg_t *)DEV_REG_ADDR(line,dev_num);
		dev->command = DEV_ACK; 
		
		/* Attendo che il dispositivo sia di nuovo ready */
		while(dev->status != DEV_ST_READY);		
	}
    /* Printer */
    
    else /* Terminal */ ;  


    /* Gestione degli INTERRUPT dei device da implementare nella PHASE2 */  
}


/* 
Funzione per trovare quale dispositivo ha causato l'interrupt
HIDDEN int whichDevice(u32* bitmap) {
  int device_n = 0; 
  while (*bitmap > 1) {
    device_n++;
    *bitmap >>= 1;
  }
  return device_n;
}

*/




/*FINE INTERRUPT*/ 

/* Gestione TLB */
void tlb_handler(void){
    
}

/* Gestione PGMTRP */
void pgmtrp_handler(void){
   
}


/* SYSTEMCALL */
/* HIDDEN perché accessibili solo da sysbk_handler */

HIDDEN void getCpuTime(unsigned int* user, unsigned int* kernel, unsigned int* wallclock){
	/* Assegno il pcb corrente ad un pcb interno alla funzione e
	 * aggiorno il tempo prima di restituire il valore 
	*/
	pcb_t* pcb = outProcQ(&ready_queue, current_process);
	pcb->kernel_time += TOD_LO -pcb->kernel_time_start;
	pcb->kernel_time_start = TOD_LO;
	
	if(user)
		*user = pcb->user_time;

	if(kernel)
		*kernel = pcb->kernel_time;

	if(wallclock)
		*wallclock = TOD_LO - pcb->total_time;
}

/* SYSCALL2
 * Quando invocata crea un processo figlio del chiamante.
 * I valori di PC e $SP sono dentro *statep 
 * cpid contiene l'ID del processo figlio 
 */
HIDDEN int createProcess(state_t* statep, int priority, void** cpid){
	pcb_t* child = allocPcb();
	
	if(cpid)
		*((pcb_t **)cpid) = child;
	
	if(!child)
		return -1;

	/* Setto i valori delllo stato */
	memcpy(&child->p_s, statep, sizeof(state_t));
	
	/* Setto i valori delle priorità */
	child->original_priority = child->priority = priority;

	/* Setto i valori del tempo di esecuzione */
	child->total_time = TOD_LO;

	/* Inserisco il processo come figlio del chiamante */
	insertChild(current_process, child);
	insertProcQ(&ready_queue, child);
	
	return 0;
}



/* SYSCALL3
 * Quando invocata termina il processo corrente
 * e tutta la sua progenie, rimuovendoli dalla
 * Ready Queue.
*/
HIDDEN void terminateProcess(){
	pcb_t *p, *child;
	struct list_head *iter;
	
	/* Associo il processo corrente ad un pcb_t interno alla funzione */
	p = outProcQ(&ready_queue, current_process);
	
	/* Itero la lista dei figli del processo e li rimuovo
	 * spostandoli nella lista dei pcb liberi */
        list_for_each(iter, &p->p_child){
		child = container_of(iter, pcb_t, p_sib);
		outProcQ(&ready_queue, child);
        	freePcb(removeChild(child));
	}
	
	/* Setto il processo corrente a NULL e libero tutto il resto 
	 * decremento il process_count e quando non ci sono più processi
	 * arriva a 0*/
	current_process = NULL;
	outChild(p);
    	freePcb(p);
    	process_count--;

	/* Il controllo ritorna allo scheduler */
    	scheduler();
}

/* SYSCALL4
 * Operazione di rilascio su un semaforo
 * Il valore del semaforo è memorizzato nella variabile passata come parametro
 */
HIDDEN void verhogen(int* semaddr){
	*semaddr+=1;
	pcb_t* blocked;
	if(*semaddr <= 0){
		blocked = removeBlocked(semaddr);
		blocked->priority = blocked->original_priority;
	}
	
	if(blocked)
		insertProcQ(&ready_queue, blocked);
}



