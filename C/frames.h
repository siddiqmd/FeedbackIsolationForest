#ifndef FRAMES_INCL
#define FRAMES_INCL

#include "common.h"
#include "object.h"
#include "strfun.h"

#define CODEGEN_FRAMESTRUCT(t) typedef struct t##frame {\
    d(char*)* colnames;\
    d(char*)* rownames;\
    d(t)** data;\
    int nrow;\
    int ncol;\
    bool column_major;\
} t##frame

#define CODEGEN_INITFRAMESIG(t)\
t##frame* _init_##t##frame_(int nrow,int ncol,bool column_major,d(t)** mat)
#define CODEGEN_CUTFRAMESIG(t)\
t##frame* _cut_##t##frame_(t##frame* tocut,d(int)* majors,bool antimajor)
#define CODEGEN_SPLITFRAMESIG(t)\
t##frame* _split_##t##frame_(t##frame** tosplit,d(int)* majors,bool antimajor)
#define CODEGEN_CONVFRAMESIG(t,u)\
t##frame* _conv_##t##frame_##u##frame_(u##frame* uf)

#define CODEGEN_DECLAREFRAME(t)\
CODEGEN_FRAMESTRUCT(t);\
CODEGEN_INITFRAMESIG(t);\
CODEGEN_CUTFRAMESIG(t);\
CODEGEN_SPLITFRAMESIG(t)

CODEGEN_DECLAREFRAME(ntstring);
CODEGEN_DECLAREFRAME(int);
CODEGEN_DECLAREFRAME(long);
CODEGEN_DECLAREFRAME(float);
CODEGEN_DECLAREFRAME(double);

CODEGEN_CONVFRAMESIG(double,ntstring);

#define for_each_in_frame_by_major(m,n,ref,frame,code) for_each_in_mat(m,n,ref,(frame)->data,code)
#define for_each_in_frame(m,n,ref,frame,code) for_each_in_frame_by_major(m,n,ref,frame,code)
#define for_each_in_frame_by_row(r,c,ref,frame,code) {({\
    assign(_frame_,frame);\
    forseq(r,0,_frame_->nrow,forseq(c,0,_frame_->ncol,{({\
        assign(ref,\
            &(_frame_->data\
                [choice(_frame_->column_major,c,r)]\
                [choice(_frame_->column_major,r,c)]));\
        {code}\
    });}))\
});}
#define for_major_in_frame(m,ref,frame,code) for_row_in_mat(m,ref,(frame)->data,code)

#define init_frame(t,nrow,ncol,colmaj) _init_##t##frame_(nrow,ncol,colmaj,NULL)
#define frame(t,mat,colmaj) _init_##t##frame_(0,0,colmaj,mat)
#define cut_frame(t,frame,majors,antim) _cut_##t##frame_(frame,majors,antim)
#define split_frame(t,frame,majors,antim) _split_##t##frame_(&(frame),majors,antim)
#define conv_frame(t,u,uf) _conv_##t##frame_##u##frame_(uf)

#endif
