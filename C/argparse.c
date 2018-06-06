#include "argparse.h"

parsed_args* parse_args(int argc, char** argv) {
    const char* pname = *(argv++);
    d(option)* dopts = option_spec();
    d(option)* opts = vecalloc(option,length(dopts)+1);
    copy_into(opts,dopts);
    opts[length(opts)-1] =
        (option){
            .sarg='h',
            .larg="help",
            .name=NULL,
            .desc="Print this help message and exit.",
            .default_value=NULL,
            .value=NULL,
            .isflag=true,
            .flagged=false
        };
    dimfree(dopts);
    parsed_args* pargs;
    if (--argc) {
        opts = populate_options(opts,argc,argv);
    }
    if (argc==0||opts[length(opts)-1].flagged) print_help(pname,opts);
    pargs = validate_args(opts);
    dimfree(opts);
    return pargs;
}

d(option)* populate_options(d(option)* opts, int argc, char** argv) {
    for_each_in_vec(i,opt,opts,
        opt->value = nstrdup(opt->default_value);
    )
    while (argc--) {
        char* cur = *(argv++);
        if (cur[0]=='-') {
            if (strlen(cur)>1&&cur[1]=='-') {
                char *larg,*val;
                larg=val=cur+2;
                bool isflag,flipflag,setflag;
                isflag=flipflag=setflag=false;
                while (*val!='\0'&&*val!='=') val++;
                if (*val=='=') {
                    *val='\0';
                    val++;
                    if (streq(val,"true")) {
                        isflag=true;
                        setflag=true;
                    } else if (streq(val,"false")) {
                        isflag=true;
                    }
                } else {
                    isflag=true;
                    flipflag=true;
                }
                option* copt = NULL;
                for_each_in_vec(i,opt,opts,
                    if (streq(larg,opt->larg)) {
                        copt = opt;
                        break;
                    }
                )
                if (copt) {
                    if (isflag&&copt->isflag) {
                        copt->flagged=choice(flipflag,!(copt->flagged),setflag);
                    } else if (!isflag&&!(copt->isflag)) {
                        copt->value = nstrdup(val);
                    } else if (isflag&&!(copt->isflag)) {
                        err_and_exit(1,"Option \"%s\" is not a flag and expects a value.\n(Use option -h/--help for more info).\n",cur);
                    } else {
                        err_and_exit(1,"Option \"%s\" is a flag and does not expect a value.\n(Use option -h/--help for more info).\n",cur);
                    }
                } else {
                    err_and_exit(1,"Invalid long option name \"%s\".\n(Use option -h/--help for more info).\n",cur);
                }
            } else {
                char *sarg = cur;
                while (*(++sarg)!='\0') {
                    option* copt = NULL;
                    for_each_in_vec(i,opt,opts,
                        if (opt->sarg==*sarg) {
                            copt = opt;
                            break;
                        }
                    )
                    if (copt) {
                        if (copt->isflag) {
                            copt->flagged=!(copt->flagged);
                        } else {
                            if (argc--) {
                                char* val = *(argv++);
                                copt->value=nstrdup(val);
                            } else {
                                err_and_exit(1,"Short option \"%c\" is not a flag and expects a value.\n(Use option -h/--help for more info).\n",*sarg);
                            }
                        }
                    } else {
                        err_and_exit(1,"Unknown short option \"%c\".\n(Use option -h/--help for more info).\n",*sarg);
                    }
                }
            }
        } else {
            err_and_exit(1,"Unknown option \"%s\".\n(Use option -h/--help).\n",cur);
        }
    }
    return opts;
}

void print_help(const char* pname,d(option*) opts) {
    printf("Usage: %s [options]\nOptions:\n",pname);
    for_each_in_vec(i,opt,opts,
        if (opt->isflag) {
            printf("\t-%c, --%s\n\t\t%s\n",opt->sarg,opt->larg,opt->desc);
        } else {
            printf("\t-%c %s, --%s=%s\n\t\t%s\n",opt->sarg,opt->name,opt->larg,opt->name,opt->desc);
        }
        if (opt->default_value) {
            printf("\t\tDefault value is %s.\n",opt->default_value);
        }
    )
    exit(0);
}

d(int*) parse_multi_ints(const char* cspec) {
    int size = 0;
    char *orig,*spec;
    orig=spec=nstrdup(cspec);
    bool range=false;
    int last, inc;
    while(*spec!='\0') {
        if (str_conv_adv(&inc,int,spec)) return NULL;
        if (*spec==','||*spec=='\0') {
            if (range) {
                size+=abs(inc-last);
            }
            size++;
            range = false;
        } else if (*spec=='-') {
            if (range) return NULL;
            range = true;
            last = inc;
        } else {
            return NULL;
        }
        if (*spec!='\0') spec++;
    }
    if (range||size==0) return NULL;
    d(int*) vec = vecalloc(int,size);
    spec=orig;
    int idx=inc=0;
    while(*spec!='\0') {
        str_conv_adv(&inc,int,spec);
        if (*spec==','||*spec=='\0') {
            if (range) {
                while (last!=inc) vec[idx++]=choice(last<inc,last++,last--);
            }
            vec[idx++]=inc;
            range = false;
        } else {
            range = true;
            last = inc;
        }
        if (*spec!='\0') spec++;
    } 
    free(orig);
    return vec;
}
