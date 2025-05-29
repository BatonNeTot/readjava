#ifndef common_h
#define common_h

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros/11994395#11994395
#define FE_0(WHAT, OP)
#define FE_1(WHAT, OP, X) WHAT(X) 
#define FE_2(WHAT, OP, X, ...) WHAT(X) OP FE_1(WHAT, OP, __VA_ARGS__)
#define FE_3(WHAT, OP, X, ...) WHAT(X) OP FE_2(WHAT, OP, __VA_ARGS__)
#define FE_4(WHAT, OP, X, ...) WHAT(X) OP FE_3(WHAT, OP, __VA_ARGS__)
#define FE_5(WHAT, OP, X, ...) WHAT(X) OP FE_4(WHAT, OP, __VA_ARGS__)
#define FE_6(WHAT, OP, X, ...) WHAT(X) OP FE_5(WHAT, OP, __VA_ARGS__)
#define FE_7(WHAT, OP, X, ...) WHAT(X) OP FE_6(WHAT, OP, __VA_ARGS__)
#define FE_8(WHAT, OP, X, ...) WHAT(X) OP FE_7(WHAT, OP, __VA_ARGS__)
#define FE_9(WHAT, OP, X, ...) WHAT(X) OP FE_8(WHAT, OP, __VA_ARGS__)

#define GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME 
#define FOR_EACH(action,op,...) \
  GET_MACRO(_0,__VA_ARGS__,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0)(action,op,__VA_ARGS__)

#define swapEndian8(value) (value)
#ifdef _MSC_VER  
    #define swapEndian16(value) _byteswap_ushort(value)
    #define swapEndian32(value) _byteswap_ulong(value)
    #define swapEndian64(value) _byteswap_uint64(value)
#else
    #define swapEndian16(value) __builtin_bswap16(value)
    #define swapEndian32(value) __builtin_bswap32(value)
    #define swapEndian64(value) __builtin_bswap64(value)
#endif

#ifndef IS_BIG_ENDIAN
#define IS_BIG_ENDIAN 0
#endif

#if IS_BIG_ENDIAN
#define beToCPU(value, size) (value)
#else
#define beToCPU(value, size) swapEndian##size(value)
#endif


#define read_block(buffer, size)\
do {\
    size_t readCount = fread(buffer, 1, size, pClassFile);\
    if (readCount != size) {\
        fprintf(stderr, "readjava: Error: '%s':unexpected EOF\n", pFilename);\
        EOF_ACTION();\
    }\
} while(0)

#define read_swap_endian(var, size)\
do {\
    _Static_assert(sizeof(var) == size / 8, "");\
    uint##size##_t readValue;\
    read_block(&readValue, sizeof(readValue));\
    (var) = beToCPU(readValue, size);\
} while(0)
#define read_u8(var) read_swap_endian(var, 8)
#define read_u16(var) read_swap_endian(var, 16)
#define read_u32(var) read_swap_endian(var, 32)
#define read_u64(var) read_swap_endian(var, 64)


#define ALLOC(size) (malloc(size))
#define FREE(ptr) (free(ptr))

#define PADDING "  "
#define PD PADDING

#endif