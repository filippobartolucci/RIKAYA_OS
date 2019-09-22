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
 
#include "listx.h"
#include "types_rikaya.h"
#include "const.h"
#include "asl.h"
#include "pcb.h"

/* array di semd con dimensione massima MAX PROC*/
HIDDEN semd_t semd_table[MAXPROC];

/*lista dei semd liberi o inutilizzati*/

HIDDEN LIST_HEAD(semdFree_h);

/* Creo lista ASL e sentinella*/

HIDDEN LIST_HEAD(semd_h);

/* Inizializzo la lista dei semdFree in modo da contenere tutti gli elementi della semd_table 
*/
void initASL (void) {
	int i=0;
	for (;i<MAXPROC;i++){
		semd_t* tmp= &semd_table[i];
		list_add_tail(&(tmp->s_next),&(semdFree_h));
	}
}


/* DESCRIZIONE: 
 * restituisce il puntatore al SEMD nella ASL la cui chiave è pari a key. Se non 
 * esiste un elemento nella ASL con chiave eguale a key, viene restituito NULL. 
*/
semd_t* getSemd(int *key){
	
	if (list_empty(&semd_h)) return NULL; 
	semd_t* p; 
	semd_t* res = NULL;
	list_for_each_entry(p, &semd_h, s_next){		
		if(p->s_key == key){
			res = p;
		}
	} 
	return res; 
}

/* DESCRIZIONE: 
 * Viene inserito il PCB puntato da p nella coda dei processi bloccati 
 * associata al SEMD con chiave key. Se il semaforo corrispondente non è presente nella ASL, 
 * alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella ASL, 
 * settando I campi in maniera opportuna (i.e. key e s_procQ). Se non è possibile allocare 
 * un nuovo SEMD perché la lista di quelli liberi è vuota, restituisce TRUE. 
 * In tutti gli altri casi, restituisce FALSE.
*/
int insertBlocked(int *key, pcb_t* p){
	/* Cerco il semd con s_key corrispondente a key */
	semd_t* semd =  getSemd(key);  
	
	if (semd != NULL){ 
		/* Vedo se il semaforo cercato è presente nella ASL
		 * e inserisco il PCB puntato da p nella coda del semaforo trovato */
		insertProcQ(&(semd->s_procQ) , p);	
		return FALSE;
	}
	
	/* Se il semaforo cercato non è presente nella ASL, allora provo ad allocarne uno dalla lista libera
	* Verifico se questa è vuota, se così fosse ritorno TRUE */
	if (list_empty(&(semdFree_h))) return TRUE; 
	
	/* Prendo un semaforo dalla lista dei semafori liberi */
	semd = container_of(semdFree_h.next, semd_t, s_next);
	list_del(semdFree_h.next);
	
	/* Inizializzo le liste del semaforo e lo inserisco nella coda dei processi */
	INIT_LIST_HEAD(&semd->s_next);
	INIT_LIST_HEAD(&semd->s_procQ);
	insertProcQ(&semd->s_procQ, p);
	semd->s_key=key;
	 
	/* Scorro la lista per inserire il semaforo nella lista dei semafori in ordine decrescente di chiave */
	semd_t *i;
	list_for_each_entry(i, &semd_h, s_next) {
		if (semd->s_key > i->s_key) {
			list_add(&semd->s_next, &i->s_next);
			p->p_semkey = key;
			return FALSE;
		}
	}

	list_add(&semd->s_next, &semd_h);
	p->p_semkey = key;
	return FALSE;
	
}


/* DESCRIZIONE:
 * Creo una funzione ausiliaria per verificare se la lista s_procQ di un semaforo è vuota.
 * In caso affermativo, rimuove s_next dalla lista e lo aggiunge alla lista dei semafori liberi
*/
HIDDEN void remProcTailSem(semd_t* semd){
	if(emptyProcQ(&semd->s_procQ)){
		list_del(&semd->s_next);
		list_add(&semd->s_next, &semdFree_h);
	}
}


/* DESCRIZIONE: 
 * Rimuove il PCB puntato da p dalla coda del semaforo 
 * su cui è bloccato (indicato da p- >p_semKey). Se il PCB non compare in tale coda, 
 * allora restituisce NULL (condizione di errore). Altrimenti, restituisce p. 
*/
pcb_t* outBlocked(pcb_t *p){
	semd_t* semd = getSemd(p->p_semkey); /* Cerco il semaforo con chiave uguale a quella indicata nel pcb */
	if(semd == NULL) return NULL;

	pcb_t* pcb = outProcQ(&(semd -> s_procQ), p); /* Rimuovo pcb puntato da p dalla coda dei processi bloccati */
	pcb->p_semkey = NULL;
	
	remProcTailSem(semd);
	
	return pcb;
}


/* DESCRIZIONE: 
 * Ritorna il primo PCB dalla coda dei processi bloccati (s_ProcQ) 
 * associata al SEMD della ASL con chiave key. Se tale descrittore 
 * non esiste nella ASL, restituisce NULL. Altrimenti, restituisce 
 * l’elemento rimosso. Se la coda dei processi bloccati per il semaforo diventa vuota, 
 * rimuove il descrittore corrispondente dalla ASL e lo inserisce 
 * nella coda dei descrittori liberi (semdFree).
*/
pcb_t* removeBlocked(int *key){
	semd_t *semd = getSemd(key); /* cerco semd con chiave key */
	
	if (semd == NULL) return NULL; /* semd non è presente nella ASL */
	
	pcb_t *pcb = headProcQ(&(semd -> s_procQ));
	if (pcb == NULL) return NULL; /* coda dei processi bloccati vuota */
	
	/* Rimuovo s_procQ dalla coda dei processi 
	 * Chiamo remProcTailSem(semd) per rimuoveres_next dalla lista 
	 * aggiungendolo alla lista dei semafori liberi */
	pcb = removeProcQ(&(semd->s_procQ));

	remProcTailSem(semd);
	
	return pcb;
}


/* DESCRIZIONE:
 * Restituisce (senza rimuovere) il puntatore al PCB 
 * che si trova in testa alla coda dei processi associata al SEMD con chiave key. 
 * Ritorna NULL se il SEMD non compare nella ASL oppure se compare ma la sua coda 
 * dei processi è vuota.
*/

pcb_t* headBlocked(int *key){
	semd_t* semd = getSemd(key);
	if (semd == NULL || emptyProcQ(&semd->s_next)) return NULL; /* Il semd non è presente nella ASL */
	pcb_t* pcb = container_of((semd -> s_procQ.next), pcb_t, p_next);
	return pcb;
}

/* DESCRIZIONE:
 * Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato.
 * Elimina tutti i processi dall'albero radicato in p dalle eventuali code dei semafori su cui sono bloccati
 */
void outChildBlocked(pcb_t* p){
	/* Rimuovo p usando outBlocked dichiarata in precedenza */
	outBlocked(p);
	pcb_t *i;

	/* Scorro la lista dei fratelli e chiamo ricorsivamente outChildBlocked su ognuno */
	list_for_each_entry(i, &p->p_child, p_sib){
		outChildBlocked(i);
	}
}
