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

#define __ACC_PREFIX(name) ACC_##name
#define __ACC_PRINT(name) if ((ACC_PRINT_VAR) & ACC_##name) fprintf(ACC_PRINT_OUTPUT_STREAM, #name ACC_PRINT_PADDING);

#define fprint_acesses_or_none(...)\
do {\
    access_flags supported = FOR_EACH(__ACC_PREFIX, |, __VA_ARGS__);\
    if (((ACC_PRINT_VAR) & supported) == 0) {\
        fprintf(pStream, "---");\
    }\
    FOR_EACH(__ACC_PRINT,,__VA_ARGS__)\
} while(0)

#endif