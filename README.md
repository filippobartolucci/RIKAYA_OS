# PHASE1.5_SO1819
Fase 1.5 del progetto del corso Sistemi Operativi 18/19 UniBO 

# Progetto 
![Schema OS](/Livelli.png)

Componenti da implementare in questa fase:
* Inizializzazione del sistema
* Interrupt timer con time slice di 3ms
* Gestore delle SYSCALL e SYSCALL3 (terminateprocess)
* Scheduler con priorità ed aging


## Compilare per μMPS2
È necessario avere prima installato [μMPS2](https://github.com/tjonjic/umps).

Per compilare basterà lanciare il comando make dentro la cartella p1test_rikaya
```
$ cd p1test_rikaya 
$ make 
``` 
Verrano creati due file, kernel.core.umps e kernel.stab.umps, che sono i file da usare per la configurazione della macchina MIPS in μMPS2

# Autori
 * Bartolucci Filippo - filippo.bartolucci2@studio.unibo.it
 * Cerio Francesco - francesco.cerio@studio.unibo.it
 * Case Umberto - umberto.case@studio.unibo.it
 * Celani Matteo - matteo.celani@studio.unibo.it

 
# License 
Progetto sotto GPLv3 License - vedere LICENSE.md per i dettagli
