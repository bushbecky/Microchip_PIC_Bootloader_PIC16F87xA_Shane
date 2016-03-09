static char rcsid[] = "$Id: NTIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $";

/*
 * $Header: /cvs/www.microchipc.com/www/PIC16bootload/PIC\040bootloader/Downloader\040DOS/NTIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $
 *
 *	$Log: NTIO.C,v $
 *	Revision 1.1  2005/06/12 09:17:32  shane
 *	First version.
 *	
 * Revision 1.3  97/09/23  11:01:00  clyde
 * Add debug stuff
 * 
 * Revision 1.2  96/11/28  12:58:06  jeremy
 * gbyte now takes a timeout argument in milliseconds.
 * (but not used for NT).
 * 
 * Revision 1.1  96/07/25  11:24:38  clyde
 * Initial revision
 * 
*/

/*
 *	I/O routines for Lucifer on Windows NT console
 *
 *	The default speed is 38400 baud
 *
 */

#include	<stdio.h>
#include	<setjmp.h>
#include	<string.h>
#include	<ctype.h>

#undef	BOOL

#include	<windows.h>

HANDLE		buffd;
unsigned char	tout;
extern int _debug_level_;	/* The level of debugging required. */

static void (*	hlist[10])(int);
static unsigned	hindex;

void delay(void) {
unsigned long count;

for (count=0; count < 0xffff; ++count);
}


void
add_handler(void (* handler)(int))
{
	if(hindex == sizeof hlist/sizeof hlist[0]) {
		printf("Too many break handlers\n");
		exit(1);
	}
	hlist[hindex++] = handler;
}

BOOL WINAPI
brkhandle(DWORD type)
{
	if(hindex != 0 && (type == CTRL_C_EVENT)) {
		(*hlist[hindex-1])(0);
		return TRUE;
	}
	return FALSE;
}

void
remove_handler(void)
{
	if(hindex != 0)
		hindex--;
}

/* Function:	gbyte
 * Descrip:	Read a character, with a timeout.
 * Arguments:	t
 *			Timeout in milliseconds. (NOT USED!)
 * Returns:	Character read
 */
unsigned char
gbyte(long t)
{
	DWORD	bread;
        unsigned char   buf[2];

	tout = 0;
        buf[0] = 0xFF;
	ReadFile(buffd, buf, 1, &bread, NULL);
	if(!bread) {
		tout = 1;
		ClearCommError(buffd, &bread, NULL);

                if( _debug_level_==2) {
			printf("{t} ");
			fflush( stdout);
		}
	}
        if(bread && (_debug_level_==2)) {
		printf("{%2.2X} ",buf[0]);
		fflush( stdout);
	}
	return buf[0];
}

void
sbyte(unsigned char b)
{
	DWORD	bread;
	char	buf[2];


        //delay();
        if( _debug_level_==2) {    /* Output debug info. */
		printf("[%2.2X] ", b);
		fflush(stdout);
	}

	buf[0] = b;
	WriteFile(buffd, buf, 1, &bread, NULL);
}


void
smulti(char * buf, unsigned cnt)
{
	DWORD	bread;

	WriteFile(buffd, buf, cnt, &bread, NULL);
}

void
cflush()
{
	PurgeComm(buffd, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
}

void
comend()
{
	CloseHandle(buffd);
}

void
disint()
{
}

void
cominit(char * port, unsigned long speed) 
{
	DCB		ourdcb;
	COMMTIMEOUTS	timeout;

	buffd = CreateFile(port, GENERIC_READ|GENERIC_WRITE,
		0,		/* not shared */
		NULL,		/* no security descriptor */
		OPEN_EXISTING,	/* it's already there, right? */
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(buffd == INVALID_HANDLE_VALUE) {
		perror(port);
		exit(1);
	}
	if(!GetCommState(buffd, &ourdcb)) {
		perror("GetCommState");
		exit(1);
	}
	ourdcb.BaudRate = speed;
	ourdcb.fBinary = 1;
	ourdcb.fParity = 0;
	ourdcb.fOutxCtsFlow = 0;
	ourdcb.fOutxDsrFlow = 0;
	ourdcb.fDtrControl = DTR_CONTROL_ENABLE;
	ourdcb.fDsrSensitivity = 0;
	ourdcb.fOutX = 0;
	ourdcb.fInX = 0;
	ourdcb.fNull = 0;
	ourdcb.fRtsControl = RTS_CONTROL_ENABLE;
	ourdcb.fAbortOnError = 0;
	ourdcb.ByteSize = 8;
	ourdcb.StopBits = ONESTOPBIT;
	if(!SetCommState(buffd, &ourdcb)) {
		perror("SetCommState");
		exit(1);
	}
	SetupComm(buffd, 2048, 2048);
	timeout.ReadIntervalTimeout = 0;
	timeout.ReadTotalTimeoutMultiplier = 1;
	timeout.ReadTotalTimeoutConstant = 100;
	timeout.WriteTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 0;
	if(!SetCommTimeouts(buffd, &timeout)) {
		perror("SetCommTimeouts");
		exit(1);
	}
	if(!SetConsoleCtrlHandler(brkhandle, TRUE))
		printf("SetConsoleCtrlHandler failed: %d\n", GetLastError());
}

/*
 *	End of file: $Id: NTIO.C,v 1.1 2005/06/12 09:17:32 shane Exp $
 */
