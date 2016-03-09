#include <stdio.h>
#include <stdlib.h>

#include "gethex.h"

prog_data prog[ROM_SIZE]={0};
unsigned total;

unsigned char
gx(unsigned char c)
{
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return c - '0';
}

unsigned int
g2x(unsigned char * s)
{
	return (gx(*s) << 4) + gx(s[1]);
}

unsigned int
g4x(unsigned char * s)
{
	return (g2x(s) << 8) + g2x(s+2);
}


unsigned char LoadHex(char *fname) {
FILE *hexfile;
unsigned char linebuf[MAX_LINE], *lnbufptr;
unsigned count,addr,rectype,loop;

if ((hexfile = fopen(fname,"r"))==NULL) {
    fprintf(stderr,"Can't open hex file: %s\n",fname);
    return 0;
}

total=0;
while (fgets(linebuf,MAX_LINE,hexfile)) {
    lnbufptr = linebuf;
    if (*lnbufptr++ != ':') {
        fprintf(stderr,"Bad HEX file format, line didn't start with ':'\n");
        return 0;
    }
        count = g2x(lnbufptr);
    lnbufptr+=2;
    addr = g4x(lnbufptr);
    addr /=2;
    lnbufptr+=4;
    rectype = g2x(lnbufptr);
    lnbufptr+=2;
    if (rectype==1) {
        fclose(hexfile);
        return 1;
    }
    if (rectype>1) {
        fprintf(stderr,"Can't handle %X record type in HEX file\n",rectype);
        fclose(hexfile);
        return 0;
    }
    
    for(loop=0; loop < count/2; ++loop) {
        prog[total].address = addr;
        prog[total].data[1] = g2x(lnbufptr);
        lnbufptr+=2;
        prog[total].data[0] = g2x(lnbufptr);
        if (prog[total].data[0] > 0x3f) {
            printf("Bad HEX file format.\n\n");
            return 0;
        }
        lnbufptr+=2;
        ++addr;
        ++total;
    }
}
fclose(hexfile);
return 0;
}

