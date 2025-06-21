#ifndef access_flags_h
#define access_flags_h

#include "common.h"

typedef uint16_t access_flags;

enum __ACCESS_FLAGS {
    ACC_PUBLIC          = 	0x0001,
    ACC_PRIVATE         = 	0x0002,
    ACC_PROTECTED       = 	0x0004,
    ACC_STATIC          = 	0x0008,
    ACC_FINAL           = 	0x0010,
    ACC_SYNCHRONIZED    = 	0x0020,
    ACC_SUPER           = 	0x0020,
    ACC_VOLATILE        = 	0x0040,
    ACC_TRANSIENT       = 	0x0080,
    ACC_NATIVE          = 	0x0100,
    ACC_INTERFACE       = 	0x0200,
    ACC_ABSTRACT        = 	0x0400,
    ACC_STRICT          = 	0x0800,
};

#define fprint_acesses(var, pStream, ...)\
do {\
    fprintf(pStream, "(0x%04"PRIx16")"PD, (var));\
    access_flags supported = FOR_EACH(DUMMY, |, __VA_ARGS__);\
    access_flags flags[] = { __VA_ARGS__ };\
    const char* flagsNames[] = { FOR_EACH_COMMA(STR, __VA_ARGS__) };\
    bool printPadding = false;\
    for (int i = 0; i < sizeof(flags) / sizeof(*flags); ++i) {\
        if ((var) & flags[i]) { \
            if (printPadding) {\
                fprintf(pStream, ","PD);\
            }\
            printPadding = true;\
            \
            fprintf(pStream, flagsNames[i]);\
        }\
    }\
} while(0)

#endif