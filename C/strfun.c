#include "strfun.h"

// added for support in windows
char *strtok_r(char *s, const char *delim, char **save_ptr)
{
    char *token;
    
    if(s == NULL)
        s = *save_ptr;
    
    /* Scan leading delimiters.  */
    s += strspn(s, delim);
    if(*s == '\0')
    {
        *save_ptr = s;
        return NULL;
    }
    
    /* Find the end of the token.  */
    token = s;
    s = strpbrk(token, delim);
    
    if(s == NULL)
    {
        /* This token finishes the string.  */
        *save_ptr = strchr(token, '\0');
    }
    else
    {
        /* Terminate the token and make *SAVE_PTR point past it.  */
        *s = '\0';
        *save_ptr = s + 1;
    }
    
    return token;
}

int count_char(const char* str,char delim) {
    int count = 0;
    forseq(i,0,strlen(str),{
        if (str[i]==delim) count++;
    })
    return count;
}

d(char*)* tokenize(const char* tstr,char delim) {
    d(char*)* tkns = vecalloc(char*,count_char(tstr,delim)+1);
    char *_tstr,*_delim,*saveptr;
    _tstr = nstrdup(tstr);
    _delim = talloc(char,2);
    _delim[0]=delim;_delim[1]='\0';
    for_each_in_vec(i,token,tkns,{
        *token = nstrdup(strtok_r(choice(i==0,_tstr,NULL),_delim,&saveptr));
    })
    free(_delim);
    free(_tstr);
    return tkns;
}

#define strtol_fix(ccp,eptr) strtol(ccp,eptr,10)
#define strtoll_fix(ccp,eptr) strtoll(ccp,eptr,10)
#define CODEGEN_STRCONV(t,cfunc) CODEGEN_STRCONVSIG(t) {\
    char *str,*end;\
    end=str=*strptr;\
    *convptr = cfunc(str,&end);\
    if (advance) *strptr=end;\
    return (str==end)||choice(strict,*end!='\0',false);\
}
CODEGEN_STRCONV(int,strtol_fix)
CODEGEN_STRCONV(long,strtoll_fix)
CODEGEN_STRCONV(float,strtof)
CODEGEN_STRCONV(double,strtod)
#undef strol_fix
#undef stroll_fix
