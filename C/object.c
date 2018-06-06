
#include "object.h"

//Dim functions
dimmeta* _vecalloc_(size_t elsz,int length,dimmeta* vec) {
    if (vec==NULL) {
        vec = (dimmeta*)calloc(sizeof(dimmeta)+(elsz*length),1);
    }
    vec[0].elsz = elsz;
    vec[0].ncol = length;
    vec[0].nrow = 1;
    vec[0].ndep = 1;
    vec[0].ndim = 1;
    return vec+1;
}

dimmeta** _matalloc_(size_t elsz,int nrow,int ncol,dimmeta** mat,dimmeta* vec) {
    if (vec==NULL) {
        vec = (dimmeta*)calloc(sizeof(dimmeta)+(elsz*ncol),nrow);
    }
    if (mat==NULL) {
        mat = (dimmeta**)malloc(sizeof(dimmeta)+(__SIZEOF_POINTER__*nrow));
    }
    dimmeta** ret = (dimmeta**)bytewalk(mat,sizeof(dimmeta));
    dim(ret)[0].elsz = elsz;
    dim(ret)[0].ncol = ncol;
    dim(ret)[0].nrow = nrow;
    dim(ret)[0].ndep = 1;
    dim(ret)[0].ndim = 2;
    int i;
    for (i=0;i<nrow;i++) {
        dimmeta* vecp = (dimmeta*)bytewalk(vec,i*(sizeof(dimmeta)+elsz*ncol));
        ret[i] = _vecalloc_(elsz,ncol,vecp);
    }
    return ret;
}

dimmeta*** _dim3alloc_(size_t elsz,int ndep,int nrow,int ncol) {
    dimmeta* vec = (dimmeta*)calloc(sizeof(dimmeta)+(elsz*ncol),nrow*ndep);
    dimmeta** mat = (dimmeta**)malloc((sizeof(dimmeta)+(__SIZEOF_POINTER__*nrow))*ndep);
    dimmeta*** dim3 = (dimmeta***)malloc(sizeof(dimmeta)+(__SIZEOF_POINTER__*ndep));
    dimmeta*** ret =  (dimmeta***)bytewalk(dim3,sizeof(dimmeta));
    dim(ret)[0].elsz = elsz;
    dim(ret)[0].ncol = ncol;
    dim(ret)[0].nrow = nrow;
    dim(ret)[0].ndep = ndep;
    dim(ret)[0].ndim = 3;
    int i;
    for (i=0;i<ndep;i++) {
        dimmeta** matp = (dimmeta**)bytewalk(mat,i*(sizeof(dimmeta)+(__SIZEOF_POINTER__*nrow)));
        dimmeta* vecp = (dimmeta*)bytewalk(vec,i*((sizeof(dimmeta)+(elsz*ncol))*nrow));
        ret[i] = _matalloc_(elsz,nrow,ncol,matp,vecp);
    }
    return ret;
}

void _dimfree_(pointer ptr) {
    pointer old;
    switch (ndim(ptr)) {
        case 3:
            old = ptr;
            ptr = binary_cast(pointer,ptr);
            free(dim(old));
        case 2:
            old = ptr;
            ptr = binary_cast(pointer,ptr);
            free(dim(old));
        default:
            free(dim(ptr));
    }
}

dimmeta* _dim_copy_(dimmeta* dest,dimmeta* src) {
    if (!dest) {
        switch (ndim(src)) {
            case 3:
                dest = (dimmeta*)_dim3alloc_(elsz(src),ndep(src),nrow(src),ncol(src));
                break;
            case 2:
                dest = (dimmeta*)_matalloc_(elsz(src),nrow(src),ncol(src),NULL,NULL);
                break;
            default:
                dest = (dimmeta*)_vecalloc_(elsz(src),length(src),NULL);
        }
    }
    dimmeta dest_meta = *dim(dest);
    dimmeta* dest_ptr = dest;
    dimmeta* src_ptr = src;
    while (ndim(dest_ptr)>1) {
        dest_ptr = (dimmeta*)(*((dimmeta**)dest_ptr));
        src_ptr = (dimmeta*)(*((dimmeta**)src_ptr));
    }
    dest_ptr = dim(dest_ptr);
    src_ptr = dim(src_ptr);
    size_t dest_bytes,src_bytes;
    dest_bytes = (nels(dest)*elsz(dest))+(sizeof(dimmeta)*(ndep(dest)*nrow(dest)));
    src_bytes = (nels(src)*elsz(src))+(sizeof(dimmeta)*(ndep(src)*nrow(src)));
    memcpy(dest_ptr,src_ptr,min(dest_bytes,src_bytes));
    *dim(dest) = dest_meta;
    return dest;
}
//End dim functions

//List functions
struct listnode {
    pointer el;
    listnode* next;
    listnode* prev;
};
#define node_attach(p,n) ({\
    if (p) p->next = n;\
    if (n) n->prev = p;\
})

int _list_in_(list* lst,pointer el,bool head) {
    listnode* inc = talloc(listnode,1);
    inc->el = el;
    if (lst->size==0) {
        lst->ptr = inc;
        node_attach(inc,inc);
    } else {
        node_attach(lst->ptr->prev,inc);
        node_attach(inc,lst->ptr);
        if (head) lst->ptr = inc;
    }
    return ++(lst->size);
}

pointer _list_out_(list* lst, bool head, bool remove, bool iterate) {
    if (lst->size == 0) return (pointer)NULL;
    listnode* target = choice(head,lst->ptr,lst->ptr->prev);
    pointer el = target->el;
    if (remove) {
        lst->ptr = target->next;
        node_attach(target->prev,lst->ptr);
        if (lst->size == 1) lst->ptr = NULL;
        free(target);
        (lst->size)--;
    } else if (iterate) {
        lst->ptr = choice(head,target->next,target);
    }
    return el;
}

void list_free(list* lst,bool free_els) {
    pointer ptr;
    while (lst->size>0) {
        ptr = list_consume(pointer,lst);
        if (free_els) free((pointer*)ptr);
    }
    free(lst);
}
#undef node_attach

struct hash_kv {
    bool occupied,extended;
    p_int key;
    p_int value;
};

hash_kv* _hash_get_kv_(hash_table* ht,p_int key) {
    size_t mkey = key%nbin(ht);
    hash_kv* kv = (ht->table)+mkey;
    if (kv->extended) return _hash_get_kv_(binary_cast(hash_table*,kv->value),key);
    return kv;
}

hash_table* make_hash_table(size_t nbins) {
    hash_table* ht = talloc(hash_table,1);
    ht->table = vecalloc(hash_kv,nbins);
    ht->keylist = talloc(list,1);
    return ht;
}

bool _hash_in_(hash_table* ht, p_int key, p_int value, bool top) {
    bool success = true;
    size_t mkey = key%nbin(ht);
    hash_kv* kv = (ht->table)+mkey;
    if (kv->occupied) {
        if (kv->extended) {
            success = _hash_in_(binary_cast(hash_table*,kv->value),key,value,false);
        } else if (kv->value==value&&kv->key==key) {
            success = false;
        } else if (kv->key==key) {
            kv->value = value;
            success = false;
        } else {
            hash_table* nht = make_hash_table(nbin(ht)+1);
            p_int oldkey = kv->key;
            p_int oldvalue = kv->value;
            kv->extended = true;
            kv->value = binary_cast(p_int,nht);
            kv->key = binary_cast(p_int,mkey);
            success =   _hash_in_(nht,key,value,false) &&
                        _hash_in_(nht,oldkey,oldvalue,false);
        }
    } else {
        kv->key = key;
        kv->value = value;
        kv->occupied = true;
    }
    if (success&&top) {
        list_add(ht->keylist,key);
    }
    return success;
}

p_int _hash_out_(hash_table* ht, p_int key) {
    hash_kv* kv = _hash_get_kv_(ht,key);
    return kv->value;
}

bool _hash_contains_(hash_table* ht, p_int key) {
    hash_kv* kv = _hash_get_kv_(ht,key);
    return kv->occupied && kv->key==key;
}

//End list functions
