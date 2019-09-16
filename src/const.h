#ifndef _CONST_H
#define _CONST_H

/****************************************************************************
 *
 * This header file contains the global constant & macro definitions.
 *
 ****************************************************************************/

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define INT_TIMER 0x10000020
#define PSEUDO_CLOCK_TICK 100
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

typedef unsigned int u32;
typedef u32 bool;


/* Maxi number of overall (eg, system, daemons, user) concurrent processes */
#define MAXPROC 20
#define MAXPRIO 20

#define UPROCMAX 3  /* number of usermode processes (not including master proc
		       and system daemons */

#define	HIDDEN static
#define	TRUE 	1
#define	FALSE	0
#define ON 	1
#define OFF 	0
#define EOS '\0'

#define FRAME_SIZE 4096
#define WORD_SIZE 4

/* Definitions for the status register */
#define STATUS_KUc 1 << 2
#define STATUS_TE  1 << 27
#define STATUS_CU  1 << 28

/* Definisco una Macro per settare lo status all'interno delle New Area, operando con un OR bit a bit imposto:
 * - Lo stato del coprocessore ad 1
 * - Local Timer ON
 * - Kernel Mode ON
 */
#define STATUS_NEW_AREA (STATUS_CU | STATUS_TE | STATUS_KUc)


/* Definisco una Macro per settare bit a bit lo status dei processi all'interno del main, facendo un OR imposto:
 *  - Kernel Mode ON
 *  - Local Timer ON
 *  - La maschera degli Interrupt
 */
#define STATUS_P (STATUS_KUc | STATUS_TE | (0xFF00 - 0x8000))


#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define NULL ((void *) 0)

#define CR 0x0a   /* carriage return as returned by the terminal */

/* Indirizzi per le NEW/OLD AREA della ROM */
#define INT_NEWAREA 0x2000008c
#define INT_OLDAREA 0x20000000
#define TLB_NEWAREA 0x200001a4
#define TLB_OLDAREA 0x20000118
#define PGMTRAP_NEWAREA 0x200002bc
#define PGMTRAP_OLDAREA 0x20000230
#define SYSBK_NEWAREA 0x200003d4
#define SYSBK_OLDAREA 0x20000348


/* Interrupt lines used by the devices */
#define INT_PLT 1
#define INT_IT  2
#define INT_DISK 3
#define INT_TAPE 4
#define INT_UNUSED 5
#define INT_PRINTER 6
#define INT_TERMINAL 7

/* Interrupting devices bitmaps addresses  */
#define INT_BITMAP_DISK 0x1000003c
#define INT_BITMAP_TAPE 0x10000040
#define INT_BITMAP_NET 0x10000044
#define INT_BITMAP_PRINTER 0x10000048
#define INT_BITMAP_TERMINAL 0x1000004C

/* Device Commands */
#define DEV_ACK   1
#define DEV_ST_READY 1
#define DEV_NOT_INSTALLED 0

/* Terminal status */
#define TERM_BUSY 3

/* Specific type state */
#define SPEC_TYPE_SYSBP 0
#define SPEC_TYPE_TLB   1
#define SPEC_TYPE_TRAP  2

/* System Call */
#define GETCPUTIME 1
#define CREATEPROCESS 2
#define TERMINATEPROCESS 3
#define VERHOGEN 4
#define PASSEREN 5
#define WAITCLOCK 6
#define WAITIO 7
#define SETTUTOR 8
#define SPECPASSUP 9
#define GETPID 10


/*Define Time-Slice*/
#define TIMESLICE 3000

#endif
