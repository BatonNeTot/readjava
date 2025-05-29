#ifndef args_h
#define args_h

#include "class_info.h"

#include "common.h"

typedef struct args_info {
    bool classInfo;
    bool constantPool;
    bool fields;
    bool methods;
    uint32_t filenamesCount;
    const char** ppFilenames;
} args_info;

void parse_args(args_info* pArgs, int argc, char** argv, FILE* pStream);

void fprint_info(args_info* pArgs, FILE* pStream);

#endif