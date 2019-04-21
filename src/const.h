#ifndef _CONST_H
#define _CONST_H

/**************************************************************************** 
 *
 * This header file contains the global constant & macro definitions.
 * 
 ****************************************************************************/

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

typedef unsigned int u32;

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
#define STATUS_IEc 0x00000001
#define STATUS_KUc 0x00000002
#define STATUS_IEp 0x00000004
#define STATUS_KUp 0x00000008
#define STATUS_IEo 0x00000010
#define STATUS_KUo 0x00000020
#define STATUS_VMc 0x01000000
#define STATUS_VMp 0x02000000
#define STATUS_VMo 0x04000000
#define STATUS_TE  0x08000000

#define STATUS_P   0x1800FF15

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

#define TIME_SCALE *((unsigned int *)0x10000024)

/* Interrupt lines used by the devices */
#define INT_PLT 1    
#define INT_IT  2 
#define INT_DISK 3
#define INT_TAPE 4
#define INT_UNUSED 5  
#define INT_PRINTER 6
#define INT_TERMINAL 7

/* System Call */
#define TERMINATEPROCESS 3

/*Define Time-Slice*/
#define TIMESLICE 3000

#endif
