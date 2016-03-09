/* protocol for bootloader <-> downloader communication */

#define READ        0xE0
#define RACK        0xE1

#define WRITE       0xE3
#define WOK         0xE4
#define WBAD        0xE5

#define DATA_OK     0xE7
#define DATA_BAD    0xE8

#define IDENT       0xEA
#define IDACK       0xEB

#define DONE        0xED
