#include "test.h"

int main(int argc, char** argv) {
    parsed_args* pargs = parse_args(argc,argv);
    printf(
        "%s\n%s\n%d - %d\n%d - %d - %d\n%s - %s\n",
        pargs->input_name,
        pargs->output_name,
        choice(pargs->metacol,pargs->metacol[0],0),
        choice(pargs->metacol,length(pargs->metacol),0),
        pargs->ntrees,
        pargs->sampsize,
        pargs->maxdepth,
        bool_as_string(pargs->header),
        bool_as_string(pargs->verbose)
    );
    ntstringframe* sf = read_csv(pargs->input_name,false,false,true);
    printf("number of rows: %d\nnumber of columns: %d\nfirst element: %s\nlast element: %s\n",
            sf->nrow,
            sf->ncol,
            sf->data[0][0],
            sf->data[nrow(sf->data)-1][ncol(sf->data)-1]);
    for_each_in_frame_by_row(r,c,item,sf,{
        printf("%s",*item);
        if (c==sf->ncol-1) printf("\n"); else printf(" - ");
    })
    hash_table* ht = make_hash_table(11);
    for_each_in_vec(i,item,pargs->metacol,{
        int val = *item;
        bool check = hash_set(ht,val);
        printf("%d - %s - %d\n",val,bool_as_string(check),population(ht));
    })
    for_each_in_list(p_int,i,item,ht->keylist,{
        printf("%d - %td\n",i,item);
    })
    ntstringframe* mf = split_frame(ntstring,sf,pargs->metacol);
    for_each_in_frame_by_row(r,c,item,mf,{
        printf("%s",*item);
        if (c==mf->ncol-1) printf("\n"); else printf(" - ");
    })
    doubleframe* ff = conv_frame(double,ntstring,sf);
    for_each_in_frame_by_row(r,c,item,ff,{
        printf("%f",*item);
        if (c==ff->ncol-1) printf("\n"); else printf(" - ");
    })
    return 0;
}
