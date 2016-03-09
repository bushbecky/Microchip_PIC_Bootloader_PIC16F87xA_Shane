static char rcsid[] = "$Id: DOSIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $";

/*
 * $Header: /cvs/www.microchipc.com/www/PIC16bootload/PIC\040bootloader/Downloader\040DOS/DOSIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $
 *
 * $Log: DOSIO.C,v $
 * Revision 1.1  2005/06/12 09:17:32  shane
 * First version.
 *
 * Revision 1.12  96/01/15  12:07:05  clyde
 * Various fixes
 * 
 * Revision 1.11  93/02/18  14:37:32  colin
 * Improved timeouts to use the BIOS tick count instead of timing loops
 * 
 * Revision 1.10  93/02/17  11:11:47  colin
 * Modified to use 2 stop bits, added smulti() routine
 * 
 * Revision 1.9  92/08/06  11:53:01  colin
 * Added 14400 baud
 * 
 * Revision 1.8  92/07/08  10:16:40  colin
 * Added end of file marker
 * 
 * Revision 1.7  91/07/10  15:50:34  colin
 * Added 57600 and 115200 baud, reduced timeouts
 * 
 * Revision 1.6  91/01/14  14:17:07  colin
 * Double timeout
 * 
 * Revision 1.5  90/07/02  10:23:47  colin
 * Lengthened timeouts properly this time, added comend() routine which
 * releases the serial port
 * 
 * Revision 1.4  90/06/07  12:23:34  colin
 * Longer timeouts
 * 
 * Revision 1.3  90/06/07  10:57:28  colin
 * Lengthened timeouts
 * 
 * Revision 1.2  89/11/15  12:35:26  colin
 * Added some intermediate speeds like 7200 and 28800
 * 
 * Revision 1.1  89/08/21  16:39:03  colin
 * Initial revision
 * 
 */

/*
 *	Routines for Lucifer to access DOS COM ports
 */

#include	<string.h>
#include	<stdio.h>
#include	"proto.h"

#include	<ioctl.h>
#include	<conio.h>
#include	<stdlib.h>
#include	<sys.h>
#include	<intrpt.h>

unsigned char	tout;
extern int	_debug_level_;

void cflush(void);

/* Routines to access an IBM serial port. */

#define	PORT1	((port unsigned char *)0x3F8)	/* COM1 */
#define	PORT2	((port unsigned char *)0x2F8)	/* COM2 */

#define	IRQ4	(far isr *)0x30		/* for COM1 */
#define	IRQ3	(far isr *)0x2C		/* for COM2 */

#define	MSK4	0x10
#define	MSK3	0x08

#define	ICR	((port unsigned char *)0x20)	/* interrupt control register */
#define	IMR	((port unsigned char *)0x21)	/* interrupt mask register */
#define	EOI	0x20				/* end of interrupt command */

static port unsigned char *	PORT;

static struct
{
	unsigned	rate;
	unsigned char	bits;
}	baurates[] =
{
	115200,1,
	57600, 2,
	38400, 3,
	28800, 4,
	19200, 6,
	14400, 8,
	9600, 12,
	7200, 16,
	4800, 24,
	3600, 32,
	2400, 48,
	1800, 64,
	1200, 96
};

static char	inbuf[256];
static unsigned char	iptr, optr, tmp;
static isr	oldisr;

/*
** prototypes for functions not required outside of dosio.c
*/

static void interrupt	service(void);

static void
write_port(port unsigned char * pp, unsigned char value)
{
	*pp = value;
}

static unsigned char
read_port(port unsigned char * pp)
{
	return *pp;
}

static void interrupt
service(void)
{
	uchar	ch;

	inbuf[iptr] = ch = read_port(PORT);
	tmp = iptr+1;
	if(tmp != optr)
		iptr = tmp;
	write_port(ICR, EOI);
}

void
cominit(char * comname, unsigned short baud)
{
	register unsigned char	i;

	if(strcmp(comname, "com2") == 0 || strcmp(comname, "COM2") == 0)
		PORT = PORT2;
	else
		PORT = PORT1;
	for(i = 0 ; baurates[i].rate != baud ; i++)
		if(i == sizeof baurates/sizeof baurates[0]) {
			i = 1;	/* default 19200 */
			break;
		}
	oldisr = set_vector(PORT == PORT1 ? IRQ4 : IRQ3, service);
	write_port(PORT + 3, 0x80);	/* enable divisor latches */
	write_port(PORT, baurates[i].bits);
	write_port(PORT + 1, baurates[i].bits >> 8);
	write_port(PORT + 3, 7);	/* 8 bits, 2 stop, no parity */
	write_port(PORT + 4, 0x0B);	/* set DTR and RTS, enable intr */
	i = read_port(PORT);		/* clear rx buffer */
	write_port(PORT + 1, 1);	/* enable RX intr */
	write_port(IMR, read_port(IMR) & ~(PORT == PORT1 ? MSK4 : MSK3));
}


void
comend(void)
{
	write_port(PORT + 1, 0);
	write_port(PORT + 4, 0x00);	/* clear DTR and RTS, disable intr */
	write_port(IMR, read_port(IMR) | (PORT == PORT1 ? MSK4 : MSK3));
	set_vector(PORT == PORT1 ? IRQ4 : IRQ3, oldisr);
}

void
sbyte(unsigned char c)
{
        if (_debug_level_==2) {
		printf("[%2.2X] ", c);
		fflush(stdout);
	}
	while((read_port(PORT + 5) & 0x20) == 0)
		continue;
	write_port(PORT, c);
}

void
smulti(unsigned char * cp, unsigned count)
{
	uchar	ch;

	if (_debug_level_ == 2)
		printf("[");
	while (count--) {
		while((read_port(PORT + 5) & 0x20) == 0)
			continue;
		write_port(PORT, ch = *cp++);
		if (_debug_level_ == 2)
			printf("%2.2X ", ch);
	}
	if (_debug_level_ == 2)
		printf("]\n");
}

static far short	TICK_COUNT @ 0x0000046CL;

/*
 *	Read a character, delaying t ticks
 */

unsigned char
tbyte(short t)
{
	unsigned char	c;
	short		t_end, cur_t;
	int		i;

	t_end = TICK_COUNT + t;
	tout = 0;
	c = read_port(IMR);
	i = 0;
	if(c & (PORT == PORT1 ? MSK4 : MSK3)) {
		write_port(PORT + 1, 1);
		write_port(IMR, c & ~(PORT == PORT1 ? MSK4 : MSK3));
	}
	ei();
	while(iptr == optr) {
		if (++i == 10000) {
			i = 0;
			if(kbhit() && getch() == 3) {
				cflush();
                                //longjmp(intrjb, 3);
			}
		}
		if (t) {
			cur_t = t_end - TICK_COUNT;
			if (cur_t <= 0 && iptr == optr) {
                                if (_debug_level_==2) {
					printf("{t} ");
					fflush(stdout);
				}
				tout = 1;
				return 0;
			}
		}
	}
	di();
	c = inbuf[optr++];
	ei();
        if (_debug_level_==2) {
		printf("{%2.2X} ", c);
		fflush(stdout);
	}
	return c;
}

/*	Read a char, timeout in seconds */

unsigned char
gbyte(short t)
{
	return tbyte(t*18);
}





void
cflush(void)
{
	if (_debug_level_ == 2) {
		printf("{cflush} ");
		fflush(stdout);
	}
	iptr = optr;
}

/*
 *	End of file: $Id: DOSIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $
 */
