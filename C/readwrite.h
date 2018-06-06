#ifndef READWRITE
#define READWRITE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "object.h"
#include "strfun.h"
#include "frames.h"

#define ee_fopen(fname,mode) ({\
    FILE* ret = fopen(fname,mode);\
    choice(ret!=NULL,ret,({err_and_exit(1,"Could not open file: %s\n",fname);ret;}));\
})

ntstringframe* read_csv(char* fname,bool header,bool rownames,bool column_major);
char* read_next_line(FILE* file);

#endif
