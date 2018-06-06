#include "readwrite.h"

char* read_next_line(FILE* file) {
    int n = 0;
    char c;
    do {
        c=(char)fgetc(file);
        n++;
    } while(c!=EOF&&c!='\n'&&c!='\0');
    fseek(file,-n,SEEK_CUR);
    char* line = talloc(char,n);
    line = fgets(line,n,file);
    c=(char)fgetc(file);
    if (strlen(line)==0) {
        free(line);
        return NULL;
    }
    return line;
}

ntstringframe* read_csv(char* fname,bool header,bool rownames,bool column_major) {
    FILE* file = ee_fopen(fname,"rb");
    list* lines = talloc(list,1);
    char* line = read_next_line(file);
    while (line) {
        list_add(lines,line);
        line = read_next_line(file);
    }
    int linecount = lines->size;
    if (header) linecount--;
    line = list_next(char*,lines);
    line = list_next(char*,lines);
    int ncols = count_char(line,',');
    line = list_prev(char*,lines);
    line = list_prev(char*,lines);
    if (!rownames) ncols++;
    ntstringframe* csv = init_frame(ntstring,linecount,ncols,column_major);
    if (rownames) csv->rownames = vecalloc(char*,linecount);
    d(char*)* tokens;
    line = list_consume(char*,lines);
    tokens = tokenize(line,',');
    if (header) {
        csv->colnames = tokens;
        line = list_consume(char*,lines);
        tokens = tokenize(line,',');
    }
    forseq(ln,0,linecount,({
        if (rownames) {
            csv->rownames[ln] = tokens[0];
        }
        forseq(ti,0,ncols,csv->data[choice(column_major,ti,ln)][choice(column_major,ln,ti)] = tokens[ti+choice(rownames,1,0)];)
        dimfree(tokens);
        free(line);
        if (ln<(linecount-1)) {
            line = list_consume(char*,lines);
            tokens = tokenize(line,',');
        }
    });)
    return csv;
}

//End Public Definitions
