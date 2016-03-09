/*	Downloader Copyright (C)1999 HI-TECH Software.
 *	Freely distubutable.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"
#include "io.h"
#include "dosio.c"
#include "gethex.h"
#include "gethex.c"


#define VERSION "1.20"
#define MaxBuf 32

#define DEF_PORT "com1"
#define DEF_SPEED "9600"
#define MAX_RETRY 50

extern unsigned char tout;
extern prog_data prog[ROM_SIZE];
extern unsigned total;

/* prototypes */
void StartMessage(void);
void showhelp(void);
static void handle_arg(char * arg, int argtype);
unsigned char getbyte(void);
void sendbyte(unsigned char ch);
void SendDone(void);
void DownLoad(void);
void Identify(void);


int			_debug_level_;
static char *		comn = DEF_PORT;
static char *		speed = DEF_SPEED;
static char *		dlfile;
static unsigned char last=0;

#define	CL_ARG	0
#define	ENV_ARG	1

void StartMessage(void) {
/* startup message */
printf("HEX FILE DOWNLOADER %s FOR PIC16F87x\n"
       "Copyright (C) 1999 HI-TECH Software\n"
       "Modified to work with 'http://www.workingex.com/htpic' bootloader\n\n",VERSION);
}


void showhelp(void) {
/* help message */
    printf("Usage:\n"
              "\tDOWNLDR [-Sbaud_rate] [-PCOMn] [-DEBUG_MODE={1,2}] FILE.HEX\n");
}

static void
handle_arg(char * arg, int argtype)
{


	if (arg[0] == '-') {
		switch (arg[1]) {

		case 'D':
			if (!strcmp(arg, "-DEBUG_MODE=1"))
				_debug_level_ = 1;
			else if (!strcmp(arg, "-DEBUG_MODE=2"))
				_debug_level_ = 2;
			break;
		case 's':
		case 'S':
			speed = arg + 2;
			break;
		case 'p':
		case 'P':
			comn = arg + 2;
			break;
		default:
                               printf("Unknown argument %s:\"%s\"\n", (argtype == ENV_ARG)? "in environment" : "", arg);
			break;
		}
	} else if (argtype == CL_ARG) {
                   if (!dlfile) {
			dlfile = arg;
                        if (strrchr(dlfile,'.')==NULL)
                            strcat(dlfile,".HEX");
                   }
                else {
			printf("Extra argument ignored: \"%s\"\n", arg);
		}
	}
}


unsigned char getbyte(void) {
unsigned char ch,retry=MAX_RETRY;

    ch = gbyte(1);
    while(tout) {
        while (tout && retry--)
            ch = gbyte(1);
        if (tout) {
            printf("No response\n");
            sbyte(last);
            ch = gbyte(1);
            retry=MAX_RETRY;
        }
    }
    return ch;
}

void sendbyte(unsigned char ch) {
//	unsigned int delay;
//    for (delay=0; delay < 0xFFFF; ++delay);   /*fix this - seems to need small delay*/
    last = ch;
    sbyte(ch);
}


void SendDone(void) {
/* sends the 'DONE' byte */
unsigned char ch;

    printf("Activating program...");
    fflush(stdout);
    sendbyte(DONE);
    ch = getbyte();
    if (ch==WOK) {
        printf(" - OK\n");
        return;
    }
    printf("bad write [%X]\n",ch);
    exit(1);
}

void DownLoad(void) {
/* downloads the HEX file */

unsigned char buff[MaxBuf]={0};
unsigned char chk,ch,*ptr;
unsigned count,address,buftot,last,loop;

    if (!_debug_level_) {
        printf("Writing: ");
        fflush(stdout);
    }
    count=0;
    while (count < total) {   /* put MaxBuf bytes into buff array */
        ptr=buff;
        address = prog[count].address;
        buftot=0;
        chk=0;
        for(last=address; ((buftot < MaxBuf) && (count < total)); ) {
            *ptr++ = prog[count].data[0];
            *ptr++ = prog[count].data[1];
            chk+= prog[count].data[0];
            chk+= prog[count].data[1];
            buftot+=2;
            ++count;
            ++last;
            if ((prog[count].address) != last)
                break;
        }
        if (_debug_level_ == 1) {
            printf("Writing 0x%X - 0x%X (%d words)\n",address,last-1,buftot/2);
            fflush(stdout);
        }
        if (!_debug_level_) {
            printf(".");
            fflush(stdout);
        }
                            /* send the buffer to the PIC */
        sendbyte(WRITE);
        sendbyte(address>>8);  /* send the start address */
        sendbyte(address);
        sendbyte(buftot);      /* send how many bytes to write */
        sendbyte(chk);         /* send check sum */
        for(loop=0; loop < buftot; ++loop)
            sendbyte(buff[loop]);     /* send the data */
        ch = getbyte();
        if (ch != DATA_OK) {          /* data sent ok? */
            printf(" - data error [%X]\n",ch);
            exit(1);
        }
        ch = getbyte();
        if (ch != WOK) {                  /* data written ok? */
            printf(" - bad write [%X]\n",ch);
            exit(1);
        }
        fflush(stdout);
    }
    printf(" - OK\n");
}

void Identify(void) {					/* identify the bootloader */
		unsigned char inbyte;
    printf("Looking for bootloader - ");

    do{
			fflush(stdout);
			sendbyte(IDENT);
			inbyte = tbyte(2);
			printf(".");
    } while(inbyte != IDACK);
    printf(" - OK\n");
}

int main(int argc, char ** argv)
{

        StartMessage();
	dlfile = NULL;
	while (*++argv)
		handle_arg(*argv, CL_ARG);
        if (_debug_level_==1)
            printf("COMn = %s, SPEED = %s\n",comn,speed);
        if (dlfile==NULL) {
            fprintf(stderr,"No hex file specified\n");
            showhelp();
            exit(1);
        }
        cominit(comn, atoi(speed));
        cflush();
        Identify();
        if (!LoadHex(dlfile))
		exit(1);
        DownLoad();
        SendDone();
        comend();
//        printf("\nReset PIC to run newly downloaded program.\n");
        return 0;
}
