

#ifndef __GETHEX_H
#define __GETHEX_H

#define ROM_SIZE 0x2000
#define MAX_LINE 256

typedef struct {
        unsigned short address;
        unsigned char data[2];
} prog_data;

unsigned char LoadHex(char *);

#endif
