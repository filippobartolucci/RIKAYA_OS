# RIKAYA_OS
Fase 2 del progetto del corso Sistemi Operativi 18/19 UniBO

# Progetto
![Schema OS](./Livelli.png)

Componenti da implementare nella fase 2:

* Gestione delle SYSCALL
* Gestione delle INTERRUPT
* Gestione delle eccezioni

## Compilare per μMPS2
È necessario avere prima installato [μMPS2](https://github.com/tjonjic/umps) e un mipsel cross compiler.

Per compilare basterà lanciare il comando make dentro la cartella /src
```
$ make
```
Verrano creati due file, kernel.core.umps e kernel.stab.umps, che sono i file da usare per la configurazione della macchina emulata in μMPS2


# License
Progetto sotto GPLv3 License - vedere LICENSE.md per i dettagli
