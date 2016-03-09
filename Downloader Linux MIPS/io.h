
#ifndef __PICDL_IO_H
#define __PICDL_IO_H

int
cominit(char * comname, unsigned int baud);

void
comend(void);

void
sbyte(unsigned char c);

void
smulti(unsigned char * cp, unsigned count);

/* timeout indicator for gbyte() & tbyte() */
extern unsigned char	tout;

unsigned char
gbyte(short t);

unsigned char
tbyte(short t);

void
cflush(void);

#endif
