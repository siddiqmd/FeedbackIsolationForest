#ifndef COMMON_INCL
#define COMMON_INCL



#if __SIZEOF_POINTER__==__SIZEOF_SHORT__
typedef unsigned short p_int;
#elif __SIZEOF_POINTER__==__SIZEOF_INT__
typedef unsigned int p_int;
#elif __SIZEOF_POINTER__==__SIZEOF_LONG__
typedef unsigned long p_int;
#elif __SIZEOF_POINTER__==__SIZEOF_LONG_LONG__
typedef unsigned long long p_int;
#endif
typedef p_int* pointer;

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define choice(c,t,e) ((c)?(t):(e))
#define assign(x,y) __typeof__(y) x = (y)
#define nulloption(n,o) ({\
    assign(_n,n);\
    choice(_n,_n,o);\
})

#define binary_cast(t,x) (((union{__typeof__(x) a; t b;}){.a=(x)}).b)

#define bool_as_string(x) choice(x,"true","false")

typedef char* ntstring;
typedef unsigned char byte;
#define bytewalk(ptr,val) ((__typeof__(ptr))(((byte*)(ptr))+(val)))

#define talloc(t,n) ((t*)calloc(sizeof(t),n))

#define cons(t,...) (t[]){__VA_ARGS__}
#define ints(...) cons(int,__VA_ARGS__)
#define longs(...) cons(long,__VA_ARGS__)
#define floats(...) cons(float,__VA_ARGS__)
#define doubles(...) cons(double,__VA_ARGS__)

#define decision(x,y,op) ({\
    assign(_x_,x);\
    assign(_y_,y);\
    choice(_x_ op _y_,_x_,_y_);\
})

#define max(a,b) decision(a,b,>)

#define min(a,b) decision(a,b,<)

#define abs(x) ({\
    assign(_x_,x);\
    choice(_x_<0,-_x_,_x_);\
})

#define add(x,y) ((x)+(y))

#define forstepseq(idx,beg,end,step,code) {\
    __typeof__((beg)+(end)+(step)) idx;\
    __typeof__(idx) _beg_ = beg;\
    __typeof__(idx) _end_ = end;\
    __typeof__(idx) _step_ = step;\
    _step_=choice(_beg_<_end_,_step_,-_step_);\
    for (idx=_beg_;choice(_beg_<_end_,_end_-idx,idx-_end_)>0;idx+=_step_) {code}\
}

#define forseq(idx,beg,end,code) forstepseq(idx,((int)beg),((int)end),1,code)

#define err_and_exit(c,...) ({\
    fprintf(stderr,__VA_ARGS__);\
    exit(c);\
})

#endif
