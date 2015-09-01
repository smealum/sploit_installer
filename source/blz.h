#ifndef _BLZFS_H_
#define _BLZFS_H_

#include <3ds.h>

#define BLZ_NORMAL    0          // normal mode
#define BLZ_BEST      1          // best mode

unsigned char *BLZ_Code(unsigned char *raw_buffer, int raw_len, unsigned int *new_len, int best);

#endif // _EXEFS_H_
