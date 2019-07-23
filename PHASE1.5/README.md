# PHASE1.5_SO1819
Fase 1.5 del progetto del corso Sistemi Operativi 18/19 UniBO 

# Progetto 
![Schema OS](./Livelli.png)

Componenti da implementare in questa fase:
* Inizializzazione del sistema
* Interrupt timer con time slice di 3ms
* Gestore delle SYSCALL e SYSCALL3 (terminateprocess)
* Scheduler con priorità ed aging

## Test
Il test consiste nell'alternare 3 processi (test1,test2 e test3) che che stampano un'immagine sul terminale
![Test PHASE1.5](./Test.png)

## Compilare per μMPS2
È necessario avere prima installato [μMPS2](https://github.com/tjonjic/umps).

Per compilare basterà lanciare il comando make dentro la cartella PHASE1.5_SO1819/src
```
$ cd PHASE1.5_SO1819/src
$ make 
``` 
Verrano creati due file, kernel.core.umps e kernel.stab.umps, che sono i file da usare per la configurazione della macchina emulata in μMPS2

# Autori
 * Bartolucci Filippo - filippo.bartolucci2@studio.unibo.it
 * Cerio Francesco    - francesco.cerio@studio.unibo.it

 
# License 
Progetto sotto GPLv3 License - vedere LICENSE.md per i dettagli
