/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		     	*/

#include "handler.h"

extern u32 debug;

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
		old_state->reg_t9 += WORD_SIZE;
		memcpy(&current_process->p_s, old_state, sizeof(state_t));

    /* Registro nel quale è salvata la SYSCALL chiamata */
    u32 syscall_number = old_state->reg_a0;

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
		    getCpuTime((u32*) old_state->reg_a1,(u32*) old_state->reg_a2,(u32*) old_state->reg_a3);
		    break;
	    case CREATEPROCESS:
		    flag = createProcess((state_t*)old_state->reg_a1, (int)old_state->reg_a2, (void **)old_state->reg_a3);
		    break;
	    case TERMINATEPROCESS:
		    flag = terminateProcess((void **) old_state->reg_a1);
		    break;
		case PASSEREN:
			Passeren((int*)old_state->reg_a1);
			break;
		case VERHOGEN:
			Verhogen((int*)old_state->reg_a1);
			break;
		case WAITCLOCK:
			Wait_Clock();
			break;
		case WAITIO:
			Do_IO(old_state->reg_a1, (u32 *)old_state->reg_a2, old_state->reg_a3);
			break;
		case SETTUTOR:
			Set_Tutor();
      		break;
		case SPECPASSUP:
			flag = Spec_Passup(old_state->reg_a1, (state_t *)old_state->reg_a2, (state_t *)old_state->reg_a3);
			break;
		case GETPID:
			Get_pid_ppid((void **) old_state->reg_a1, (void **) old_state->reg_a1);
			break;

		default:
			/* Gestore livello superiore */
			if (!current_process->spec_set[SPEC_TYPE_SYSBP])
				terminateProcess(0);
			memcpy(old_state, current_process->spec_oarea[SPEC_TYPE_SYSBP], sizeof(state_t));
			LDST(current_process->spec_narea[SPEC_TYPE_SYSBP]);
    }
    /* Se c'è un processo viene caricato, altrimento ci pensa lo scheduler */
		if (current_process){
			/* Valore di ritorno della SYSCALL */
			old_state->reg_v0 = flag;
			/*Gestione del tempo dei processi */
			current_process->kernel_time += TOD_LO - current_process->kernel_time_start;
			current_process->user_time_start = TOD_LO;
			current_process->kernel_time_start = 0;

			/* Continuo con l'esecuzione del processo */
    	LDST(old_state);
    }
    scheduler();
}


/* Gestione INTERRUPT */
S32 semd_keys[8][7];
S32 waitc_sem = 0;

void int_handler(void){
		/* Gestione tempo */
    if (current_process->user_time_start){
	    /* Solo per processi non nuovi */
	    current_process->user_time += TOD_LO - current_process->user_time_start;
	    current_process->user_time_start = 0;
		}
    /* Stato dell'esecuzione prima dell'eccezione */
    state_t *old_state = interrupt_oldarea;
    /* Causa dell'interrupt */
    u32 cause = old_state->cause;
	int devnum = 0;
    int line = 0;

    /* Struttura per il dispositivo */
    dtpreg_t *dev;
		/* Struttura per il terminale */
		termreg_t *term;
		pcb_t* freed;

    cause = cause >> 8;
    /* Ricerca dell' interrupt
     * Controlli fatti in ordine di priorità.
     * I bit meno significativi hanno priorità
     * maggiore. Il controllo confronta
     * la causa dell'interrupt con una causa in
     * cui il bit del device che si sta controllando è a 1.
    */

    if (cause == (cause | 0x1))          /* 00000001 */
        /* IPI */
        *((u32*) 0x10000400) = 1;

    else if (cause == (cause | 0x2))     /* 00000010 */
				/* Processor Local Timer */
        scheduler();

    else if (cause == (cause | 0x4)){    /* 00000100 */
        /* Interval Timer */
        while(Verhogen((int*)&waitc_sem));
        *((u32 *)INT_TIMER) = (u32)100000 * PSEUDO_CLOCK_TICK;
    }

    else if (cause == (cause | 0x8)){   /* 00001000 */
			line =3;
			devnum = whichDevice(INT_BITMAP_DISK);
			dev = (dtpreg_t *)(DEV_REG_ADDR(line,devnum));
			freed =Verhogen((int* )&semd_keys[line][devnum]);
			freed->p_s.reg_v0 = dev->status;
			/* Invio acknowledgement interrupt */
			dev->command = DEV_ACK;
			while((dev->status) != DEV_ST_READY);
	    /* Disk */
		}
    else if (cause == (cause | 0x10)){	 /* 00010000 */
			line =4;
			devnum = whichDevice(INT_BITMAP_TAPE);
			dev = (dtpreg_t *)(DEV_REG_ADDR(line,devnum));
			freed =Verhogen((int*)&semd_keys[line][devnum]);
			freed->p_s.reg_v0 = dev->status;
			/* Invio acknowledgement interrupt */
			dev->command = DEV_ACK;
			while((dev->status) != DEV_ST_READY);
    	/* Tape */
		}

    else if (cause == (cause | 0x20)) {   /* 00100000 */
			line = 5;
			devnum = whichDevice(INT_BITMAP_NET);
			dev = (dtpreg_t *)(DEV_REG_ADDR(line,devnum));
			freed =Verhogen((int*)&semd_keys[line][devnum]);
			freed->p_s.reg_v0 = dev->status;
			/* Invio acknowledgement interrupt */
			dev->command = DEV_ACK;
			while((dev->status) != DEV_ST_READY);
    	/* Interrupt Line 5 */
		}

    else if (cause == (cause | 0x40)){   /* 01000000 */
			line = 6;
			devnum = whichDevice(INT_BITMAP_PRINTER);
			dev = (dtpreg_t *)(DEV_REG_ADDR(line,devnum));
			freed =Verhogen((int*)&semd_keys[line][devnum]);
			freed->p_s.reg_v0 = dev->status;
			/* Invio acknowledgement interrupt */
			dev->command = DEV_ACK;
			while((dev->status) != DEV_ST_READY);;
    	/* Printer */
		}

    else if (cause == (cause | 0x80)){
      line = INT_TERMINAL;
      devnum = whichDevice(INT_BITMAP_TERMINAL);
      term = (termreg_t *)DEV_REG_ADDR(line, devnum);

        if((term->recv_status & 0xFF) == 5){
			line++;
			freed = Verhogen((int*)&semd_keys[line][devnum]);
        	freed->p_s.reg_v0 = term->recv_status;
	    	insertProcQ(&ready_queue, freed);
	    	term->recv_command = DEV_ACK;
        	while((term->recv_status & 0xFF) != DEV_ST_READY);
        }else if((term->transm_status & 0xFF) == 5){
            freed = Verhogen((int*)&semd_keys[line][devnum]);
            freed->p_s.reg_v0 = term->transm_status;
            term->transm_command = DEV_ACK;
            while((term->transm_status & 0xFF) != DEV_ST_READY);
        }
    }else{
        /* Interrupt, ma nessuna linea pending */
        PANIC();
    }
    if(current_process){
        current_process->user_time_start = TOD_LO;
        LDST(old_state);
    }
    else{
        scheduler();
    }
}

/* Funzione per trovare quale dispositivo ha causato l'interrupt */
HIDDEN inline int whichDevice(u32 bitmap) {
    int dev_n = 0;
    for(; dev_n<8; dev_n++ ){
        if( bitmap && (1UL << dev_n ) )
            break;
    }
    return dev_n;
}

/* Gestione TLB */
void tlb_handler(void){
    state_t* old_state = tlbmgt_oldarea;
    old_state->pc_epc += WORD_SIZE;
    old_state->reg_t9 += WORD_SIZE;

    /* Se il processo non ha un handler viene terminato */
    if (!current_process->spec_set[SPEC_TYPE_TLB])
        terminateProcess(NULL);

    memcpy(old_state, current_process->spec_oarea[SPEC_TYPE_TLB], sizeof(state_t));
    LDST(current_process->spec_narea[SPEC_TYPE_TLB]);
}

/* Gestione PGMTRP */
void pgmtrp_handler(void){
    state_t* old_state = program_trap_oldarea;
    old_state->pc_epc += WORD_SIZE;
    old_state->reg_t9 += WORD_SIZE;

    /* Se il processo non ha un handler viene terminato */
    if (!current_process->spec_set[SPEC_TYPE_TRAP])
        terminateProcess(NULL);

    memcpy(old_state, current_process->spec_oarea[SPEC_TYPE_TRAP], sizeof(state_t));
    /* L'handler che si occuperà della trap */
    LDST(current_process->spec_oarea[SPEC_TYPE_TRAP]);
}


/* SYSTEMCALL */

/* SYSCALL 1
 * Quando invocata restituisce il tempo di esecuzione del processo
 * che l'ha chiamata fino a quel momento, separato in tre variabili:
 *  - Tempo usato dal processo come utente (user)
 *  - Tempo usato dal processo come kernel (tempi di SYSCALL e INTERRUPT relativi al processo
 *  - Tempo totale trascorso dalla prima attivazione del processo
*/
HIDDEN void getCpuTime(u32* user, u32* kernel, u32* wallclock){
    /* Assegno il pcb corrente ad un pcb interno alla funzione e
     * aggiorno il tempo prima di restituire il valore
     */

    current_process->kernel_time += TOD_LO -current_process->kernel_time_start;
    current_process->kernel_time_start = TOD_LO;

    if(user)
        *user = current_process->user_time;

    if(kernel)
        *kernel = current_process->kernel_time;

    if(wallclock)
        *wallclock = TOD_LO - current_process->total_time;
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

HIDDEN int terminateProcess(void ** pid){
    /* PCB da terminare */
    pcb_t *victim = NULL;
    /* Determino la vittima */
    if (pid == NULL || pid == 0){
        victim = current_process;
    }else {
        victim = (pcb_t *)pid;
    }

    pcb_t *tut = current_process;
    /* Se il processo vittima è diverso dal processo corrente */
    if (pid!= NULL){
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

    /* Rilascio dell'eventuale semaforo della vittima*/
    if (victim->p_semkey){
        *(victim->p_semkey)+=1;
        /* Rimuovo la vittima dalla coda del semaforo */
        outBlocked(victim);
    }
    /* Rimuovo la vittima dalla lista dei figli del padre */
    outChild(victim);
    /* Rimuovo la vittima dalla ready_queue */
    outProcQ(&ready_queue, victim);
    /* Restituisco il pcb alla lista libera */
    freePcb(victim);
		state_t *old_state = sysbk_oldarea;
		old_state->reg_v0 = 0;
    return 0;
}

/* SYSCALL4
 * Operazione di rilascio su un semaforo
 * Il valore del semaforo è memorizzato nella variabile passata come parametro
 */
HIDDEN pcb_t* Verhogen(int* semaddr){
    *semaddr+=1;
    pcb_t* blocked=NULL;
	/* Controllo se ci sono procecssi in attesa sul semaforo */
	if (*semaddr <= 0){
		blocked = removeBlocked(semaddr);
		blocked->priority = blocked->original_priority;
	}

	if(blocked)
		insertProcQ(&ready_queue, blocked);

	return blocked;

}

/* SYSCALL5
 * Operazione di richiesta di un semaforo.
 * Il valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L'indirizzo della variabile agisce da identificatore per il semaforo
*/
HIDDEN void Passeren(int *semaddr){
    *semaddr-=1;
    if (*semaddr<0){
        /* Rimuovo il processo dalla ready_queue */
        outProcQ(&ready_queue,current_process);
        /* Aggiungo il processo alla coda del semaforo */
        insertBlocked(semaddr,current_process);
        /* Copio lo stato di esecuzione del processo */
		state_t *old_state = interrupt_oldarea;
		memcpy(old_state, &current_process->p_s, sizeof(state_t));

        current_process = NULL;
    }

}

/* SYSCALL6
 * Sospende il processo che la invoca fino
 * al prossimo tick del clock di sistema
*/
HIDDEN void Wait_Clock(void){
    if(waitc_sem == 0){
       *((u32 *)INT_TIMER) = (u32)100000 * TIME_SCALE;// (u32)PSEUDO_CLOCK_TICK;
    }
    Passeren(&waitc_sem);
}

/* SYSCALL 7
 * Attiva un'operazione di I/O copiando nel parametro
 * command nel campo comando del registro del dispositivo
 * indicato come puntatore nel secondo argomento.
 * Il valore restituito è il contenuto del registro di status del dispositivo
*/
HIDDEN int Do_IO(u32 command, u32* reg, int transm){
	dtpreg_t *devreg = (dtpreg_t *) reg;
	termreg_t *termreg = (termreg_t*) reg;

	/* Ricerca del device */
	int line=0;
  	int devn=0;

	int status;
	/* Confronto il device passato come parametro con tutti i device installati */
	for(int i=3;i<8;i++){
		for(int j=0;j<8;i++){
			if (DEV_REG_ADDR(i,j) == (u32)devreg){
				line = i;
				devn = j;
                break;
			}if(line) break;
		}
  	}
	/* Il device non è un terminale */
	if (line < 7) {
		/* Ottengo il puntatore al device */
	    devreg = (dtpreg_t *)reg ;
		/* Mando il commando */
	    devreg->command = command ;
		/* P sul semaforo del device*/
	    Passeren((int*)&(semd_keys[line][devn]));
	    status = devreg->status;
	}
	/* Il device è un terminale */
	else if (line == 7) {
	    termreg = (termreg_t*)reg ;
	    if (transm== FALSE){
					/* Comando transm */
	        termreg->transm_command = command ;
	        Passeren((int*)&(semd_keys[line][devn]));
	        status = termreg->transm_status ;
        }
        else {
          	/* Commando receive */
          	termreg->recv_command = command ;
          	Passeren((int*)&(semd_keys[line][devn]));
          	status =  termreg->recv_status ;
        }
    }

	return status;
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
 * Il significato dei parametri old e new è lo stesso delle
 * aree old e new gestite dal codice della ROM:
 * quando avviene una trap da passare al gestore lo stato
 * del processo che ha causato la trap viene posto nell’area
 * old e viene caricato lo stato presente nell’area new.
 * La system call deve essere richiamata una sola volta per tipo.
 * Se la system call ha successo restituisce 0, altrimenti -1.
*/
HIDDEN int Spec_Passup(int type, state_t *old, state_t *new){
  if(current_process->spec_set[type]){
		return -1;
	}

  memcpy(current_process->spec_oarea[type], old, sizeof(state_t));
	memcpy(current_process->spec_narea[type], new, sizeof(state_t));

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
