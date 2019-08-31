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

	  /* Gestione del tempo dei processi */
    if (current_process->user_time_start){
		    /* Solo per processi non nuovi */
		    current_process->user_time += TOD_LO - current_process->user_time_start;
    	  current_process->user_time_start = 0;
	  }
	  current_process->kernel_time_start = TOD_LO;

    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = sysbk_oldarea;

	  /* Viene incrementato il valore del PC */
	  old_state->pc_epc += WORD_SIZE;

    /* Registro nel quale è salvata la SYSCALL chiamata */
    u32 syscall_number = old_state->reg_a0;

	  /* Causa dell'eccezione */
	  u32 *cause = old_area->cause;

    /* Parametri della SYSCALL */
    u32 *arg1 =  &old_state->reg_a1;
    u32 *arg2 =  &old_state->reg_a2;
    u32 *arg3 =  &old_state->reg_a3;

    /* Controllo Breakpoint */
	  if (syscall_number == 9) {
    	if (!current_process->spec_set[SPEC_TYPE_SYSBP])
    		terminateProcess(0);
    	memcpy(old_state, current_process->spec_oarea[SPEC_TYPE_SYSBP], sizeof(state_t));
	    LDST(current_process->spec_narea[SPEC_TYPE_SYSBP]);
  	}


	  /* SYSCALL */
    int flag = 0;

    switch (syscall_number){
        /* Eseguo la SYSCALL richiesta */
		      case GETCPUTIME:
			        getCpuTime((u32*) arg1,(u32*) arg2,(u32*) arg3);
	 		        break;

		      case CREATEPROCESS:
	  	 	       flag = createProcess((state_t*)arg1, (int)old_state->reg_a2, (void **)arg3);
	   	 	       break;

		      case TERMINATEPROCESS:
              flag = terminateProcess((void **) arg1);
              break;

		      case PASSEREN:
      		    Passeren((int *) arg1);
      		    break;

  		    case VERHOGEN:
    	 	      Verhogen((int *) arg1);
      		    break;

		      case WAITCLOCK:
      		    Wait_Clock();
 		          break;

  /* SYSCALL7 chiamata Do_IO nelle specifiche, ma
   * definita come WAITIO nel file const.h e test
  */
    	    case WAITIO:
      		     Do_IO();
      		     break;

    	    case SETTUTOR:
      		     Set_Tutor();
      		     break;

    	    case SPECPASSUP:
     		       flag = Spec_Passup();
      		     break;

    	    case GETPID:
      		     Get_pid_ppid((void **) arg1, (void **) arg2);
      		     break;

          default:
            /* Errore numero SYSCALL inesistente */
            PANIC();
    }

	  /* Valore di ritorno della SYSCACLL */
    old_state->reg_v0 = flag;

	  /*Gestione del tempo dei processi */
	  current_process->p_kernelt_total += TOD_LO - current_process->p_kernelt_start;
  	current_process->p_kernelt_start = 0;
  	current_process->p_usert_start = TOD_LO;

    if (current_process)
        LDST(&old_state);
    else scheduler();
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
        /* Inter-processor-interrupts */

    else if (cause == (cause | 0x2))     /* 00000010 */
		/* Processor Local Timer */
        scheduler();

    else if (cause == (cause | 0x4))     /* 00000100 */
        line = 2;
        /* Interval timer */

    else if (cause == (cause | 0x8))     /* 00001000 */
		    line = 3;
        /* Disk */

    else if (cause == (cause | 0x10))	 /* 00010000 */
		    line = 4;
        /* Tape */

    else if (cause == (cause | 0x20))    /* 00100000 */
		    line = 5;
        /* Network */

    else if (cause == (cause | 0x40)){   /* 01000000 */
		    /* Printer */
		    line = 6;
        /* Cerco quale stampante ha causato l'interrupt */

        int dev_num = whichDevice((u32*)INT_BITMAP_PRINTER);

		    // Codice per i semafori e processi bloccati ai semafori

		    /* Invio l'ACK alla stampante */
		    dev = (dtpreg_t *)DEV_REG_ADDR(line,dev_num);
		    dev->command = DEV_ACK;

		    /* Attendo che sia di nuovo ready */
		    while(dev->status != DEV_ST_READY);
	  }


    else /* Terminal */ ;

    if (current_process)
        LDST(&old_state);
    else scheduler();

}


/* Funzione per trovare quale dispositivo ha causato l'interrupt */
HIDDEN int whichDevice(u32* bitmap) {
  int dev_n = 0;
  while (*bitmap > 1) {
    dev_n++;
    *bitmap >>= 1;
  }
  return dev_n;
}




/*FINE INTERRUPT*/ 


/* Gestione TLB */
void tlb_handler(void){
    state_t* old = tlbmgt_oldarea;
    old->pc_epc += WORD_SIZE;
}

/* Gestione PGMTRP */
void pgmtrp_handler(void){
    state_t* old = program_trap_oldarea;
    old->pc_epc += WORD_SIZE;
}


/* SYSTEMCALL */
/* HIDDEN perché accessibili solo da sysbk_handler */

/* SYSCALL 1
 * Quando invocata restituisce il tempo di esecuzione del processo
 * che l'ha chiamata fino a quel momento, separato in tre variabili:
 *  - Tempo usato dal processo come utente (user)
 *  - Tempo usato dal processo come kernel (tempi di SYSCALL e INTERRUPT relativi al processo
 *  - Tempo totale trascorso dalla prima attivazione del processo
*/
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
 * Quando invocata termina il processo identificato dal PID
 * (corrente se PID == 0 o NULL), ma non la sua progenie. I processi
 * figli vengono adottati dal primo antenato che sia marcato come tutor.
 * Restituiscce 0 se ha successo e -1 in caso di errore
*/
HIDDEN void terminateProcess(void ** pid){
    /* PCB da terminare */
    pcb_t *victim = NULL;
    
    /* Determino la vittima */
    if (pid == NULL || pid == 0){
        victim = current_process;
    }else {
        victim = (pcb_t *)pid;
    }
    
    /* Controllo se la vittima è il processo root */
    if (victim->p_parent == NULL)
        return -1;
    
    pcb_t *tut = current_process;
    
    
    // DA CONTROLLARE 
    /* Se il processo vittima è diverso dal processo corrente */
    if (pid!= NULL || pid !=0){
        /* Controllo che la vittima sia un discendente del processo corrente */
        while ((tut = tut->p_parent)){
            if (tut == current_process)
                break;
            /* Altrimenti restituisco un errore */
            else if (tut->p_parent == NULL)
                return -1;
        }
    }
 
    /* Trovo un pcb che possa fare da tutore ai figli della vittima */
    tut = victim;
    while (tut->tutor != TRUE)
        tut = tut->p_parent;
    
    /* Assegno tutti i figli della vittima al pcb tutore */
    pcb_t *child = NULL;
    while ((child = removeChild(victim))!= NULL)
        insertChild(tut, child);
    
    /* Rimuovo la vittima dalla lista dei figli del padre */
    outChild(victim);
    /* Rimuovo la vittima dalla ready_queue */
    outProcQ(&ready_queue, victim);
    
    //------------------------------------------------------------
    /* Rilascio dell'eventuale semaforo della vittima
    if (victim->p_semKey)
        (*victim->p_semKey)++; */
    //------------------------------------------------------------
     
    /* Rimuovo la vittima dalla coda del semaforo */
    outBlocked(victim);
    /* Restituisco il pcb alla lista libera */
    freePcb(victim);
    
    
    if (pid == NULL || pid == 0){
        /* Se il chiamante si è suicidato il controllo va allo scheduler */
        current_process = NULL;
        scheduler();
    }
    
    /* Controllo ritorna al chiamante */
    return 0;
}

/* SYSCALL4
 * Operazione di rilascio su un semaforo
 * Il valore del semaforo è memorizzato nella variabile passata come parametro
 */
HIDDEN void Verhogen(int* semaddr){
	termprint("sono nella verhogen\n", 0);
	*semaddr+=1;
	pcb_t* blocked;
	if(*semaddr <= 0){
		blocked = removeBlocked(semaddr);
		blocked->priority = blocked->original_priority;
	}
	
	if(blocked)
		insertProcQ(&ready_queue, blocked);
}


/* SYSCALL5
 * Operazione di richiesta di un semaforo.
 * Il valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L'indirizzo della variabile agisce da identificatore per il semaforo
*/
HIDDEN void Passeren(int *semaddr){
    *semaddr--;
    if (*semaddr<0){
        /* Rimuovo il processo dalla ready_queue */
        outProcQ(&ready_queue,current_process);
        /* Aggiungo il processo alla coda del semaforo */
        insertBlocked(semaddr,current_process);
        /* Copio lo stato di esecuzione del processo */
        memcpy(old_state, &current_process->p_s, sizeof(state_t));
        /* Faccio avanzare il PC */
        current_process->p_s.pc_epc += WORD_SIZE;

        current_process == NULL;
    }

}

/* SYSCALL6
 * Sospende il processo che la invoca fino
 * al prossimo tick del clock di sistema
*/
HIDDEN void Wait_Clock(void){
    
}

/* SYSCALL 7
 * Attiva un'operazione di I/O copiando nel parametro
 * command nel campo comando del registro del dispositivo
 * indicato come puntatore nel secondo argomento.
 * Il valore restituito è il contenuto del registro di status del dispositivo
*/
HIDDEN int Do_IO(u32 command, u32* reg){
	dtpreg_t *reg1 = (dtpreg_t *) reg;
	reg1->command = command;
	return (reg1->status);
}

/* SYSCALL 8
 * Indica al kernel che il processo che la invoca
 * deve agire da tutor per i procecssi discendenti
 * che dovessero trovarsi orfani
*/
HIDDEN void Set_Tutor(){
    current_process->tutor = TRUE;
}


/* SYSCALL 9
 * Questa chiamata registra quale handler di livello superiore
 * debba essere attivato in caso di trap di Syscall/breakpoint
 * (type=0), TLB (type=1) o Program trap (type=2).
 * Il significato dei parametri old e new è lo stesso delle
 * aree old e new gestite dal codice della ROM:
 * quando avviene una trap da passare al gestore lo stato
 * del processo che ha causato la trap viene posto nell’area
 * old e viene caricato lo stato presente nell’area new.
 * La system call deve essere richiamata una sola volta per tipo.
 * Se la system call ha successo restituisce 0, altrimenti -1.
*/
HIDDEN int Spec_Passup(int type, state_t *old, state_t *new){
	memcpy(current_process->spec_oarea[type], old, sizeof(state_t));
	memcpy(current_process->spec_narea[type], new, sizeof(state_t));

	if(current_process->spec_set[type]){
		return -1;
	}

	current_process->spec_set[type] = TRUE;
	return 0;
}


/* SYSCALL 10
 * Assegna il l’identificativo del processo corrente
 * a *pid (se pid != NULL) e l’identificativo del
 * processo genitore a *ppid (se ppid != NULL)
*/
HIDDEN void Get_pid_ppid(void ** pid, void ** ppid){
    if (pid)  *pid = current_process;
    if (ppid) *ppid = current_process->p_parent;
}
