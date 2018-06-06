#ifndef ARGPARSE
#define ARGPARSE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "object.h"
#include "strfun.h"


typedef struct option {
    char sarg;
    char* larg;
    char* name;
    char* desc; 
    char* default_value;
    char* value;
    bool isflag;
    bool flagged;
} option;
//Program specific, to be defined by user elsewhere.
struct parsed_args;
typedef struct parsed_args parsed_args;
d(option)* option_spec();
parsed_args* validate_args(d(option)* opts);
//Defined in argparse.c
d(option)* populate_options(d(option)* opts,int argc, char** argv);
parsed_args* parse_args(int argc, char** argv);
void print_help(const char* pname,d(option)* opts);
d(int)* parse_multi_ints(const char* spec);

#endif
