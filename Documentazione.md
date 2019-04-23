# Documentazione PHASE 1.5_SO18-19 
Fase 1.5 del progetto per il corso di sistemi operativi 18/19 UniBo, gruppo <i>lso19az22</i>.
Obbiettivo di questa fase è implementare (in parte) le seguenti componenti:

 1. L'inizializzazione del Sistema
 2. Un interrupt timer con time slice di 3 ms
 3. Una System Call (<b>TERMINATE PROCESS</b>)
 4. Uno Scheduler con priorità ed aging


# Descrizione dei file 
## initArea.c
Questo file contiene l'inizializzazione delle diverse aree della ROM. Tramite la funzione <b>initArea()</b> vengono chiamate le seguenti funzioni:
 ```
  initSYSBK();
  initPGMTRP();
  initINT();
  initTLB();
```
Le quali si occupano di settare i giusti valori di <b>STATUS, PC</b> e <b>$SP</b> per le specifiche aree.

## handler.c
In questo file vengono gestite le System Call e i Breakpoint (questi ultimi non richiesti per questa fase).
Le seguenti funzioni si occupano di gestire le System Call e gli Interrupt
```
 - sysbk_handler();
 - int_handler();
```

La funzione 
 ```
 terminateProcess();
 ```
si occupa di terminare il processo corrente e tutta la sua progenie, rimuovendoli dalla lista <b>ready_queue</b>.

## scheduler.c
Lo scheduler si occupa di alternare i processi nel giusto ordine in base alla priorità di essi e tramite la funzione
```
  priorityAging(void);
```
la loro priorità viene incrementata.

Abbiamo definito una funzione ausiliare
```
checkEmptyProc(void);
```
che viene chiamata all'inizio dello <b>scheduler</b>.
Il suo compito è quello di controllare se è presente un processo corrente e il contatore di processi non sia uguale a 0. In caso negativo chiama la funzione di sistema <b>HALT()</b>.

## utils.c
In questo file sono definite funzioni funzioni ausiliarie per la gestione della memoria, quali
```
memset(void *s, u32 c, int n);
memcpy(void *dest, void *src, u32 n);
```

## const.h
Questo file contiene le definizioni delle costanti, aree della ROM, etc...
In particolare la macro 
```
#define STATUS_NEW_AREA (STATUS_CU | STATUS_TE | STATUS_KUc)
```
serve per settare lo status delle new area in maniera tale da avere 
 * Kernel Mode ON
 * Coprocessore ON
 * Abilitare il Local Timer
 
e la macro 
```
#define STATUS_P (STATUS_KUc | STATUS_TE | (0xFF00 - 0x8000))

```
serve per settare lo status dei processi in maniera tale da avere 
 * Kernel Mode ON
 * Abilitare il Local Timer
 * Mascherare gli Interrupt
 
#Sviluppatori
 * Bartolucci Filippo - filippo.bartolucci2@studio.unibo.it
 * Cerio Francesco - francesco.cerio@studio.unibo.it
