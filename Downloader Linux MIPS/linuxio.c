static char rcsid[] = "$Id: dosio.c,v 1.12 96/01/15 12:07:05 clyde Exp $";

/*
 *	Routines for Lucifer to access Linux RS-232 ports
 */

#include	"io.h"
#include	"proto.h"

#include	<string.h>
#include	<stdio.h>

#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<fcntl.h>
#include 	<termios.h>     

#include <errno.h>                                              

/* shared variables */
unsigned char	tout;
extern int	_debug_level_;

/* 
 * Useful constants
 */
static struct
{
	unsigned long	rate;
	unsigned int	bits;
}	baudrates[] =
{
	115200, B115200,
	57600,  B57600,
	38400,  B38400,
	19200,  B19200,
	9600,   B9600,
	4800,   B4800,
	2400,   B2400,
	1800,   B1800,
	1200,   B1200
};

/* 
 * variable global to the module 
 */
static struct termios 	oldtio, newtio;
static int 		fd;

/*
 * prototypes for functions not required outside of this module
 */


int
cominit(char * comname, unsigned int baud)
{
    unsigned int	i;
    
    for(i = 0 ; baudrates[i].rate != baud ; i++)
	if(i == sizeof baudrates/sizeof baudrates[0]) {
	    i = 3;	/* default 19200 */
	    break;
	}

    fd = open(comname, O_RDWR | O_NOCTTY );
    if (fd < 0 ) {
	perror(comname);
	return 1;
    }
    if (tcgetattr(fd,&oldtio)) { /* save current port settings */ 
	perror(comname);
	return 1;
    }
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = 
	baudrates[i].bits | /* baud rate */
	CS8 | /* 8 bits, no parity, 1 stop bits CSTOPB */
	CLOCAL | /* local connection,  ignore modem control lines */
	CREAD; /* enable receiving characters */
    newtio.c_iflag = IGNPAR; /* ignore bytes with parity errors */
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    
    newtio.c_cc[VTIME] = 1;   /* 1/10 second inter-character timer */
    newtio.c_cc[VMIN]  = 0;    /* or blocking read until 1 chars received */
    
    tcflush(fd, TCIFLUSH);   
    if (tcsetattr(fd,TCSANOW,&newtio)) {
	perror("Serial parms setup");
	return 1;
    }

    return 0;
}


void
comend(void)
{
    /* restore old port settings */
    tcsetattr(fd,TCSANOW,&oldtio);
}

void
sbyte(unsigned char c)
{
    if (_debug_level_==2) {
	printf("[%2.2X] ", c);
	fflush(stdout);
    }

    write(fd, &c, 1);
}

void
smulti(unsigned char * cp, unsigned count)
{
    if (_debug_level_==2) {
	int i;
	printf("[");
	for (i=0; i < count; i++) {
	    printf("%2.2X ", cp[i]);
	}
	printf("]\n");
	fflush(stdout);
    }

    write(fd, cp, count);
}

/*
 *	Read a character, delaying t/10 seconds
 */

unsigned char
tbyte(short t)
{
    int charsRead;
    unsigned char buf[3];

    tout = 0;  /* we have not timed out yet */
    
    /* change the timeout value if we need to */
    if ( t != newtio.c_cc[VTIME]) {
	newtio.c_cc[VTIME] = t;
	tcsetattr(fd,TCSANOW,&newtio); 
    }

    errno = 0;
    charsRead = read(fd, buf, 1);
    if (errno) {
	perror("tbyte");
    }

    if (charsRead) {
	if (_debug_level_==2) {
	    printf("{%2.2X} ", buf[0]);
	    fflush(stdout);
	}
	return buf[0];
    } else {
	if (_debug_level_==2) {
	    printf("{t} ");
	    fflush(stdout);
	}
	tout = 1;
	return 0;
    }
}

/*	Read a char, timeout in seconds */
unsigned char
gbyte(short t)
{
    return tbyte(t*10);
}

void
cflush(void)
{
    if (_debug_level_ == 2) {
	printf("{cflush} ");
	fflush(stdout);
    }
}
