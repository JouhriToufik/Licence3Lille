#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#define DATE_BUFFER_SIZE 12

struct posix_header
{                    /* Byte offset    Field type                 */
  char name[100];            /*   0    NUL-terminated if NUL fits */
  char mode[8];              /* 100                               */
  char uid[8];               /* 108                               */
  char gid[8];               /* 116                               */
  char size[12];             /* 124                               */
  char mtime[12];            /* 136                               */
  char chksum[8];            /* 148                               */
  char typeflag;             /* 156    see below                  */
  char linkname[100];        /* 157    NUL-terminated if NUL fits */
  char magic[6];             /* 257    must be TMAGIC (NUL term.) */
  char version[2];           /* 263    must be TVERSION           */
  char uname[32];            /* 265    NUL-terminated             */
  char gname[32];            /* 297    NUL-terminated             */
  char devmajor[8];          /* 329                               */
  char devminor[8];          /* 337                               */
  char prefix[155];          /* 345    NUL-terminated if NUL fits */
  char bourrage[12];         /* 500                               */
                             /* 512                               */
/* If the first character of prefix is '\0', the file name is name;
   otherwise, it is prefix/name.  Files whose pathnames don't fit in
   that length can not be stored in a tar archive.  */
};

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2
#define BUFFER 512
