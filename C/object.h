#ifndef OBJECT_INCL
#define OBJECT_INCL

#include "common.h"

//dim-style memory management

//A notational macro used to indicate an obj was/should be
//allocated with dim meta data.
#define d(x) x

typedef struct dimmeta {
    size_t elsz;
    int ncol;
    int nrow;
    int ndep;
    int ndim;
} dimmeta;

#define dim(obj) (((dimmeta*)obj)-1)
#define ncol(obj) (dim(obj)->ncol)
#define length(obj) ncol(obj)
#define nrow(obj) (dim(obj)->nrow)
#define ndep(obj) (dim(obj)->ndep)
#define ndim(obj) (dim(obj)->ndim)
#define elsz(obj) (dim(obj)->elsz)
#define nels(obj) ({\
    assign(_obj,obj);\
    ncol(_obj)*nrow(_obj)*ndep(_obj);\
})
#define vecalloc(t,len) ((t*)_vecalloc_(sizeof(t),len,NULL))
#define matalloc(t,nr,nc) ((t**)_matalloc_(sizeof(t),nr,nc,NULL,NULL))
#define dim3alloc(t,nd,nr,nc) ((t***)_dim3alloc_(sizeof(t),nd,nr,nc))
#define dimfree(ptr) _dimfree_((pointer)(ptr))
dimmeta* _vecalloc_(size_t elsz,int length,dimmeta* vec);
dimmeta** _matalloc_(size_t elsz,int nrow,int ncol, dimmeta** mat, dimmeta* vec);
dimmeta*** _dim3alloc_(size_t elsz,int ndep,int nrow,int ncol);
void _dimfree_(pointer ptr);

#define CODEGEN_PARFOR(i,ref,stop) for (;(i)<stop;(i)++,(ref)++)

#define for_each_in_vec(i,ref,vec,code) {({\
    int i=0; assign(ref,vec); int _stop_ = length(ref);\
    CODEGEN_PARFOR(i,ref,_stop_) {code}\
});}

#define for_row_in_mat(r,vec,mat,code) {({\
    int r=0; assign(vec,mat); int _stop_ = nrow(vec);\
    CODEGEN_PARFOR(r,vec,_stop_) {code}\
});}

#define for_each_in_mat(r,c,ref,mat,code) for_row_in_mat(r,_vec_,mat,for_each_in_vec(c,ref,*(_vec_),code))

#define for_mat_in_dim3(d,mat,obj,code) {({\
    inr d=0; assign(mat,obj); int _stop_ = ndep(mat);\
    CODEGEN_PARFOR(d,mat,_stop_) {code}\
});}

#define for_row_in_dim3(d,r,vec,obj,code) for_mat_dim3(d,_mat_,obj,for_row_in_mat(r,vec,*(_mat_),code))

#define for_each_in_dim3(d,r,c,ref,obj,code) for_row_in_dim3(d,r,_vec_,obj,for_each_in_vec(c,ref,*(_vec_),code))

#define for_each_in_obj(iv,ref,obj,code) {({\
    assign(_obj_,obj);\
    int[] iv = ints(0,0,0); int[] _stop_ = ints(ndep(_obj_),nrow(_obj_),ncol(_obj_));\
    if (ndim(_obj_)==3) {\
        assign(_mat_,_obj_);\
        CODEGEN_PARFOR(iv[0],_mat_,_stop_[0]) {\
            assign(_vec_,*_mat_);\
            CODEGEN_PARFOR(iv[1],_vec_,_stop_[1]) {\
                assign(ref,*_vec_);\
                CODEGEN_PARFOR(iv[2],ref,_stop_[2]) {code}\
            }\
        }\
    } else if (ndim(_obj_)==2) {\
        assign(_vec_,_obj_);\
        CODEGEN_PARFOR(iv[1],_vec_,_stop_[1]) {\
            assign(ref,*_vec_);\
            CODEGEN_PARFOR(iv[2],ref,_stop_[2]) {code}\
        }\
    } else {\
        assign(ref,_obj_);\
        CODEGEN_PARFOR(iv[2],ref,_stop_[2]) {code}\
    }\
});}
#define for_row_in_obj(iv,ref,obj,code) {({\
    assign(_obj_,obj);\
    int[] iv = ints(0,0,0); int[] _stop_ = ints(ndep(_obj_),nrow(_obj_),ncol(_obj_));\
    if (ndim(_obj_)==3) {\
        assign(_mat_,_obj_);\
        CODEGEN_PARFOR(iv[0],_mat_,_stop_[0]) {\
            assign(ref,*_mat_);\
            CODEGEN_PARFOR(iv[1],ref,_stop_[1]) {code}\
        }\
    } else {\
        assign(ref,_obj_);\
        CODEGEN_PARFOR(iv[1],ref,_stop_[1]) {code}\
    }\
});}
#define for_mat_in_obj(iv,ref,obj,code) {({\
    assign(_obj_,obj);\
    int[] iv = ints(0,0,0); int[] _stop_ = ints(ndep(_obj_),nrow(_obj_),ncol(_obj_));\
    assign(ref,_obj_);\
    CODEGEN_PARFOR(iv[0],ref,_stop_[0]) {code}\
});}

#define copy_of(src) ((__typeof__(src))copy_into(NULL,src))
#define copy_into(dest,src) _dim_copy_((dimmeta*)dest,(dimmeta*)src)

dimmeta* _dim_copy_(dimmeta* dest,dimmeta* src);

#define vseqapply(vec,fn) ({\
    assign(_vec_,vec);\
    assign(cval,*_vec_);\
    forseq(i,1,length(_vec_),cval=fn(cval,_vec_[i]);)\
    cval;\
})
#define vmin(vec) vseqapply(vec,min)
#define vmax(vec) vseqapply(vec,max)
#define vsum(vec) vseqapply(vec,add)


//End dim-style

//Circular list management
typedef struct listnode listnode;
typedef struct list {
    int size;
    listnode* ptr;
} list;

#define list_push(lst,el) _list_in_(lst,binary_cast(pointer,el),true)
#define list_add(lst,el) _list_in_(lst,binary_cast(pointer,el),false)
#define list_in(lst,el,head) _list_in_(lst,binary_cast(pointer,el),(head))
#define list_next(t,lst) binary_cast(t,_list_out_(lst,true,false,true))
#define list_prev(t,lst) binary_cast(t,_list_out_(lst,false,false,true))
#define list_peek(t,lst) binary_cast(t,_list_out_(lst,true,false,false))
#define list_head(t,lst) list_peek(t,lst)
#define list_tail(t,lst) binary_cast(t,_list_out_(lst,false,false,false))
#define list_pop(t,lst) binary_cast(t,_list_out_(lst,true,true,false))
#define list_consume(t,lst) list_pop(t,lst)
#define list_remove(t,lst) binary_cast(t,_list_out_(lst,false,true,false))
#define list_out(t,lst,head,remove,iterate) binary_cast(t,_list_out_(lst,head,remove,iterate))

int _list_in_(list* lst,pointer el,bool head);
pointer _list_out_(list* lst,bool head,bool remove,bool iterate);
void list_free(list* lst,bool free_els);

#define for_each_in_list(t,i,el,lst,code) {({\
    assign(_lst_,lst);\
    forseq(i,0,_lst_->size,{\
        t el = list_next(t,_lst_);\
        code\
    })\
});}

//End circular list

//Hash management

typedef struct hash_kv hash_kv;
typedef struct hash_table {
    d(hash_kv)* table;
    list* keylist;
} hash_table;

#define hash_set(ht,kv) ({\
    p_int _kv_ = binary_cast(p_int,kv);\
    _hash_in_(ht,_kv_,_kv_,true);\
})
#define hash_map(ht,k,v) _hash_in_(ht,binary_cast(p_int,k),binary_cast(p_int,v),true)
#define hash_retrieve(t,ht,k) binary_cast(t,_hash_out_(ht,binary_cast(p_int,k)))
#define hash_contains(ht,k) _hash_contains_(ht,binary_cast(p_int,k))

hash_table* make_hash_table(size_t size);
bool _hash_in_(hash_table* ht, p_int key, p_int value, bool top);
p_int _hash_out_(hash_table* ht, p_int key);
bool _hash_contains_(hash_table* ht, p_int key);

#define nbin(ht) length(ht->table)
#define population(ht) (ht->keylist->size)

#endif
