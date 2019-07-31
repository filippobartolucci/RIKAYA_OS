/*						*
 *		 PHASE1 RIKAYA	   		*
 *						*
 * 	 Sviluppato dal gruppo lso19az22 	*
 *						*
 * 	 Componenti del gruppo:	   		*
 *	   - Filippo Bartolucci	   		*
 *	   - Francesco Cerio		  	*
 *	   - Umberto Case		   	*
 *	   - Matteo Celani		   	*/

#include "types_rikaya.h"
#include "const.h"
#include "pcb.h"

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN LIST_HEAD(pcbFree_h); 		/* Lista con sentinella dei pcb liberi o inutilizzati */



/* PCB free list handling functions */

/* Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table. 
 * Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. 
*/
void initPcbs(void){
	int i = 0;
	for(; i < MAXPROC; i++){
		pcb_t* pcb= &pcbFree_table[i];
		list_add_tail(&(pcb->p_next),&(pcbFree_h));
	}
}

/* Inserisce il pcb puntato da p nella lista dei PCB liberi 
*/
void freePcb(pcb_t *p){
	    list_add_tail(&(p->p_next), &(pcbFree_h));
}


/* Restituisce NULL se la pcbFree è vuota.                                 
 * Altrimenti rimuove un elemento dalla pcbFree,                          
 * inizializza tutti i campi (NULL\0) e restituisce l'elemento rimosso    
*/
pcb_t *allocPcb(void){
	/* Controllo per verificare se pcbFree sia vuoto o no */
	if (list_empty(&(pcbFree_h))) return NULL;
	
	/* Elemento precedente alla sentinella è l'ultimo elemento della lista 
	 * Salvo il puntatore del PCB da puntare 
	*/
	pcb_t *tmp=container_of(pcbFree_h.prev, pcb_t, p_next);
					       
	/* Rimuove tmp dalla lista pcbFree_H che lo contiene */
	list_del(&(tmp->p_next));

	/* Inizializzo tutti i campi di tmp a NULL */
	tmp->p_parent = NULL;
	tmp->p_semkey = NULL;
	tmp->priority = 0;
	tmp->p_s.entry_hi = 0;
	tmp->p_s.cause = 0;
	tmp->p_s.status = 0;
	tmp->p_s.pc_epc = 0;
	
	/* Per il campo p_s.gpr[] uso un for per settare a 0 tutti le 29 variabili dell' array */
	int j = 0;
	for(;j<29;j++){
		tmp->p_s.gpr[j] = 0;
	}	
	tmp->p_s.hi = 0;
	tmp->p_s.lo = 0;
							        
	INIT_LIST_HEAD(&(tmp->p_next));								    
	INIT_LIST_HEAD(&(tmp->p_child));								        
	INIT_LIST_HEAD(&(tmp->p_sib));
									    
	return tmp;
 }

/* PCB queue handling functions */

/*Inizializza la lista dei PCB, inizializzando l'elemento sentinella */
void mkEmptyProcQ(struct list_head *head){
	INIT_LIST_HEAD(head);
}

/* Controllo se la lista puntata da head è vuota e restituisco true in caso affermativo, false altrimenti 
*/
int emptyProcQ(struct list_head *head){
	if (list_empty(head))
		return TRUE;
	else 
		return FALSE;
}

/* Inserisco un processo in coda alla lista, controllando che la priorità sia decrescente 
*/
void insertProcQ(struct list_head *head, pcb_t *p){
	pcb_t *tmp;
	
	list_for_each_entry(tmp, head, p_next){
		if (p->priority > tmp->priority){
			list_add_tail(&(p->p_next), &(tmp->p_next));
			return;
		}
	}
	list_add_tail(&(p->p_next), head);

}


/* Restituisce l'elemento di testa della coda dei processi da head, senza rimuoverlo. 
 * Ritorna NULL se la coda non ha elementi 					      
*/
pcb_t *headProcQ(struct list_head *head){

		/* Controllo iniziale per vedere se head è vuoto */
		if (list_empty(head)) return NULL;		
		/* Devo restituire il primo elemento della lista head non vuota. */	
		return (container_of(head->next, pcb_t, p_next)); 
}

/* Rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota. 
 * Altrimenti ritorna il puntatore all'elemento rimosso dalla lista 				       
*/
pcb_t *removeProcQ(struct list_head *head){

		/* Controllo iniziale per vedere se head è vuoto */
		/* Se lista è vuota restituisco NULL */
		if (list_empty(head)) return NULL; 
				
		/* C'è almeno un elemento nella lista */
		/* Rimuovo il primo e lo restituisco  */
		pcb_t *tmp = container_of(head->next,pcb_t,p_next) ;	/* Puntatore al primo elemento della lista */
		list_del(head->next);					/* Lo rimuovo dalla lista */
		return tmp ;
}

/* Rimuove il PCB puntato da p dalla coda dei processi puntata da head 
 * Se p non è presente nella coda, restituisce NULL	 		
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p){
	
	if (emptyProcQ(head)) return NULL;
	
	/* Scorro la lista head alla ricerca di p */
	/* Puntatore che uso per confrontare gli elementi della coda con p */
	pcb_t *tmp=NULL;	
	
	/* Se p=tmp, allora rimuovo p dalla coda e lo restituisco */
	list_for_each_entry(tmp, head, p_next){
		if (p == tmp) {	
			/* p trovato, lo rimuovo da head */ 
			list_del(&(p->p_next));
			return p;
		}
	}
	/* Ho finito di scorrere head e non ho trovato p */
	/* Restituisco NULL */
	return NULL; 
}



/* Tree view functions */


/* Controllo se la lista passata come parametro ha figli o meno e restituisco TRUE in caso non ne abbia e FALSE altrimenti 
*/ 
int emptyChild(pcb_t *this){
	if (list_empty(&(this->p_child))) 
		return TRUE;
	else 
		return FALSE;
}


/* Inserisce il PCB puntato da p come figlio del PCB puntato da prnt */
void insertChild(pcb_t *prnt, pcb_t *p){

	/* Collego il figlio con il padre tramite il puntatore *p_parent */
	p->p_parent = prnt;
	/* Aggiungo p in coda alla lista dei figli di prnt */
	list_add_tail(&(p->p_sib), &(prnt->p_child));
	
}


/* Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL 
*/
pcb_t *removeChild(pcb_t *p){
	
	/*Controllo se il processo ha dei figlio e restituisco NUll in caso negativo */ 
	if (list_empty(&(p->p_child)))	return NULL;
		
	/* Cerco il puntatore al primo figlio */
        struct list_head* firstChild = list_next(&(p->p_child));
        pcb_t* firstPcb = container_of(firstChild, pcb_t, p_sib);

        /* Elimino il puntatore al padre */
        firstPcb->p_parent = NULL;

        /* Elimino il figlio dalla lista */
        list_del(firstChild);	

        /* Restituisco il figlio eliminato */
	return firstPcb;
}



/* Rimuove il PCB puntato da p dalla lista dei figli del padre. 
 * Se il PCB puntato da p non ha un padre, restituisce NULL. 
 * Altrimenti restituisce l’elemento rimosso (cioè p). 
 * A differenza della removeChild, p può trovarsi in una posizione arbitraria 
*/
pcb_t *outChild(pcb_t *p){
	if ((&(p->p_parent))== NULL) return NULL;  
	/*elimino elemento dall'albero*/
	list_del (&(p->p_sib));
	p->p_parent = NULL;
	/*restituisco elemento da rimuovere*/
	return p;
}
