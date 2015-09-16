#include "argparse_iforest.h"

#define NOPTS 9
#define IOPT 0
#define OOPT 1
#define MOPT 2
#define TOPT 3
#define SOPT 4
#define DOPT 5
#define HOPT 6
#define VOPT 7
#define WOPT 8
#define COPT 8

d(option)* option_spec() {
    d(option)* opts = vecalloc(option,NOPTS);
    opts[IOPT] = (option){
        .sarg = 'i',
        .larg = "infile",
        .name = "FILE",
        .desc = "Specify path to input data file. (Required).",
        .default_value = NULL,
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[OOPT] = (option){
        .sarg = 'o',
        .larg = "outfile",
        .name = "FILE",
        .desc = "Specify path to output results file. (Required).",
        .default_value = NULL,
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[MOPT] = (option){
        .sarg = 'm',
        .larg = "metacol",
        .name = "COLS",
        .desc = "Specify columns to preserve as meta-data. (Separated by ',' Use '-' to specify ranges).",
        .default_value = NULL,
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[TOPT] = (option){
        .sarg = 't',
        .larg = "ntrees",
        .name = "N",
        .desc = "Specify number of trees to build.",
        .default_value = "100",
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[SOPT] = (option){
        .sarg = 's',
        .larg = "sampsize",
        .name = "S",
        .desc = "Specify subsampling rate for each tree. (Value of 0 indicates to use entire data set).",
        .default_value = "2048",
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[DOPT] = (option){
        .sarg = 'd',
        .larg = "maxdepth",
        .name = "MAX",
        .desc = "Specify maximum depth of trees. (Value of 0 indicates no maximum).",
        .default_value = "0",
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[HOPT] = (option){
        .sarg = 'H',
        .larg = "header",
        .name = NULL,
        .desc = "Toggle whether or not to expect a header input.",
        .default_value = "true",
        .value = NULL,
        .isflag = true,
        .flagged = true
    };
    opts[VOPT] = (option){
        .sarg = 'v',
        .larg = "verbose",
        .name = NULL,
        .desc = "Toggle verbose ouput.",
        .default_value = "false",
        .value = NULL,
        .isflag = true,
        .flagged = false
    };
    opts[WOPT] = (option){
        .sarg = 'w',
        .larg = "windowsize",
        .name = "N",
        .desc = "specify window size.",
        .default_value = "512",
        .value = NULL,
        .isflag = false,
        .flagged = false
    };
    opts[COPT] = (option){
        .sarg = 'c',
        .larg = "columns",
        .name = "N",
        .desc = "specify number of columns to use.",
        .default_value = "0",
        .value = NULL,
        .isflag = false,
        .flagged = false
    };

    return opts;
}

parsed_args* validate_args(d(option*) opts) {
    parsed_args* pargs = talloc(parsed_args,1);
    pargs->input_name = opts[IOPT].value;
    if (pargs->input_name==NULL) err_and_exit(1,"Must specify path to input with option -i/--infile.\n");
    pargs->output_name = opts[OOPT].value;
    if (pargs->output_name==NULL) err_and_exit(1,"Must specify path to output with option -o/--outfile.\n");
    if (opts[MOPT].value) {
        pargs->metacol = parse_multi_ints(opts[MOPT].value);
        if (pargs->metacol==NULL) {
            err_and_exit(1,"Invalid specification of meta columns.");
        }
        for_each_in_vec(i,cn,pargs->metacol,(*cn)--;)
    }
    if (str_conv_strict(&(pargs->ntrees),int,opts[TOPT].value)) {
        err_and_exit(1,"Expected integer as number of trees.\n");
    }
    if (pargs->ntrees<1) {
        err_and_exit(1,"Number of trees must be at least 1.\n");
    }
    if (str_conv_strict(&(pargs->sampsize),int,opts[SOPT].value)) {
        err_and_exit(1,"Expected integer as sample size.\n");
    }
    if (pargs->sampsize<3&&pargs->sampsize!=0) {
        err_and_exit(1,"Sample size must be at least 3.\n");
    }
    if (str_conv_strict(&(pargs->maxdepth),int,opts[DOPT].value)) {
        err_and_exit(1,"Expected integer as maximum depth.\n");
    }
    if (pargs->maxdepth<0) {
        err_and_exit(1,"Maximum depth can't be negative.\n");
    }
    pargs->sampsize = strtol(opts[SOPT].value,NULL,10);
    pargs->maxdepth = strtol(opts[DOPT].value,NULL,10);
    pargs->header = opts[HOPT].flagged;
    pargs->verbose = opts[VOPT].flagged;
    pargs->window_size = strtol(opts[WOPT].value,NULL,10);
    pargs->columns = strtol(opts[COPT].value,NULL,10);
    return pargs;
}
