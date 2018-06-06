#include "frames.h"

#define CODEGEN_INITFRAME(t) CODEGEN_INITFRAMESIG(t) {\
    nrow=choice(mat,choice(column_major,ncol(mat),nrow(mat)),nrow);\
    ncol=choice(mat,choice(column_major,nrow(mat),ncol(mat)),ncol);\
    t##frame* frame = talloc(t##frame,1);\
    frame->column_major = column_major;\
    frame->data = nulloption(mat,matalloc(t,choice(column_major,ncol,nrow),choice(column_major,nrow,ncol)));\
    frame->nrow = nrow;\
    frame->ncol = ncol;\
    return frame;\
}

#define CODEGEN_CUTFRAME(t) CODEGEN_CUTFRAMESIG(t) {\
    t##frame* ret = NULL;\
    if (!antimajor) {\
        d(t*)* cutmajors = vecalloc(t*,length(majors));\
        for_each_in_vec(newidx,oldidxptr,majors,cutmajors[newidx]=tocut->data[*oldidxptr];)\
        *dim(cutmajors)=*dim(tocut->data);\
        nrow(cutmajors)=length(majors);\
        ret = frame(t,cutmajors,tocut->column_major);\
    } else {\
        d(t**) cutanti = matalloc(t,nrow(tocut->data),length(majors));\
        for_each_in_mat(m,n,ref,cutanti,*ref=tocut->data[m][majors[n]];)\
        ret = frame(t,cutanti,tocut->column_major);\
    }\
    if (ret->column_major!=antimajor) {\
        ret->rownames=tocut->rownames;\
        ret->colnames=vecalloc(ntstring,length(majors));\
        for_each_in_vec(i,ref,ret->colnames,*ref=nstrdup(tocut->colnames[majors[i]]);)\
    } else {\
        ret->colnames=tocut->colnames;\
        ret->rownames=vecalloc(ntstring,length(majors));\
        for_each_in_vec(i,ref,ret->rownames,*ref=nstrdup(tocut->rownames[majors[i]]);)\
    }\
    return ret;\
}

#define CODEGEN_SPLITFRAME(t) CODEGEN_SPLITFRAMESIG(t) {\
    if (!majors) return NULL;\
    t##frame* oldframe = *tosplit;\
    t##frame* postcut = _cut_##t##frame_(oldframe,majors,antimajor);\
    hash_table* umaj = make_hash_table(nrow(oldframe->data));\
    for_each_in_vec(i,ref,majors,{\
            hash_set(umaj,*ref);\
    })\
    int oldpos = 0;\
    forseq(newpos,0,nrow(oldframe->data),{\
        if (!antimajor) {\
            if (!hash_contains(umaj,newpos)) {\
                oldframe->data[oldpos]=oldframe->data[newpos];\
                oldpos++;\
            }\
        } else {\
            forseq(np,0,ncol(oldframe->data),{\
                if (!hash_contains(umaj,np)) {\
                    oldframe->data[newpos][oldpos]=oldframe->data[newpos][np];\
                    oldpos++;\
                }\
            })\
            if (newpos!=(nrow(oldframe->data))-1) oldpos=0;\
        }\
    })\
    choice(antimajor,ncol(oldframe->data)=oldpos,nrow(oldframe->data)=oldpos);\
    d(ntstring)* axislabel = NULL;\
    if (oldframe->column_major!=antimajor) {\
        oldframe->ncol=oldpos;\
        axislabel = oldframe->colnames;\
    } else {\
        oldframe->nrow=oldpos;\
        axislabel = oldframe->rownames;\
    }\
    oldpos=0;\
    for_each_in_vec(i,ref,axislabel,{\
        if (!hash_contains(umaj,i)) {\
            axislabel[oldpos]=*ref;\
            oldpos++;\
        }\
    })\
    length(axislabel)=oldpos;\
    return postcut;\
}

#define CODEGEN_CONVFRAME(t,u,cfunc) CODEGEN_CONVFRAMESIG(t,u) {\
    t##frame* tf = init_frame(t,uf->nrow,uf->ncol,uf->column_major);\
    for_each_in_frame(m,n,ta,tf,cfunc(ta,uf->data[m][n]);)\
    return tf;\
}\

#define CODEGEN_DEFINEFRAME(t)\
CODEGEN_INITFRAME(t)\
CODEGEN_CUTFRAME(t)\
CODEGEN_SPLITFRAME(t)

CODEGEN_DEFINEFRAME(ntstring)
CODEGEN_DEFINEFRAME(int)
CODEGEN_DEFINEFRAME(long)
CODEGEN_DEFINEFRAME(float)
CODEGEN_DEFINEFRAME(double)

#define str_to_double_fix(fa,str) str_conv_strict(fa,double,str)
CODEGEN_CONVFRAME(double,ntstring,str_to_double_fix)
#undef str_to_float_fix

