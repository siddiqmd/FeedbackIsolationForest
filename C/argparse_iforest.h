#ifndef ARGPARSE_IFOREST
#define ARGPARSE_IFOREST

#include "argparse.h"

struct parsed_args {
    char* input_name;
    char* output_name;
    d(int*) metacol;
    int ntrees;
    int sampsize;
    int maxdepth;
    bool header;
    bool verbose;
    int window_size;
};


#endif
