#ifndef STRFUN_INCL
#define STRFUN_INCL

#include "common.h"
#include "object.h"
#include <string.h>

#define streq(x,y) !strcmp(x,y)
#define nstrdup(s) ({\
    assign(_s_,s);\
    choice(_s_,strdup(_s_),NULL);\
})
#define str_conv(aa,t,str) str_to_##t(aa,&(str),false,false)
#define str_conv_adv(aa,t,str) str_to_##t(aa,&(str),true,false)
#define str_conv_strict(aa,t,str) str_to_##t(aa,&(str),false,true)

int count_char(const char* cstr,char delim);
d(char*)* tokenize(const char* tstr,char delim);

#define CODEGEN_STRCONVSIG(t) bool str_to_##t(t* convptr,char** strptr,bool advance,bool strict)
CODEGEN_STRCONVSIG(int);
CODEGEN_STRCONVSIG(long);
CODEGEN_STRCONVSIG(float);
CODEGEN_STRCONVSIG(double);

#endif
