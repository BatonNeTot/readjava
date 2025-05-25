#ifndef common_h
#define common_h

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

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

#define IS_BIG_ENDIAN 0

#if IS_BIG_ENDIAN
#define beToCPU(value, size) (value)
#else
#define beToCPU(value, size) swapEndian##size(value)
#endif



#define readBlock(buffer, size)\
do {\
    size_t readCount = fread(buffer, 1, size, pClassFile);\
    if (readCount != size) {\
        fprintf(stderr, "Unexpected EOF.\n");\
        exit(-1);\
    }\
} while(0)

#define readAndSwap(var, size)\
do {\
    _Static_assert(sizeof(var) == size / 8, "");\
    uint##size##_t readValue;\
    readBlock(&readValue, sizeof(readValue));\
    (var) = beToCPU(readValue, size);\
} while(0)
#define readU8(var) readAndSwap(var, 8)
#define readU16(var) readAndSwap(var, 16)
#define readU32(var) readAndSwap(var, 32)
#define readU64(var) readAndSwap(var, 64)


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

#define print_access(accessFlags, formatPrefix, accessName, formatPostfix, ...)\
if ((accessFlags) & ACC_##accessName) printf(formatPrefix #accessName formatPostfix)
#define printf_access(accessFlags, formatPrefix, accessName, formatPostfix, ...)\
if ((accessFlags) & ACC_##accessName) printf(formatPrefix #accessName formatPostfix, __VA_ARGS__)

#endif