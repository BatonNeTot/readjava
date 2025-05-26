#ifndef cp_info_h
#define cp_info_h

#include "common.h"

typedef uint8_t cp_tag;

enum __CP_TAG {
    CP_CLASS = 7,
    CP_FIELDREF = 9,
    CP_METHODREF = 10,
    CP_IMETHODREF = 11,
    CP_STRING = 8,
    CP_INTEGER = 3,
    CP_FLOAT = 4,
    CP_LONG = 5,
    CP_DOUBLE = 6,
    CP_NAME_AND_TYPE = 12,
    CP_UTF8 = 1
};

typedef cp_tag cp_info;

typedef struct cp_class_info {
    cp_tag tag;
    uint16_t nameIndex;
} cp_class_info;

typedef struct cp_fieldref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} cp_fieldref_info;

typedef struct cp_methodref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} cp_methodref_info;

typedef struct cp_imethodref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} cp_imethodref_info;

typedef struct cp_string_info {
    cp_tag tag;
    uint16_t stringIndex;
} cp_string_info;

typedef struct cp_integer_info {
    cp_tag tag;
    union {
    int32_t value;
    uint32_t bytes;
    };
} cp_integer_info;

typedef struct cp_float_info {
    cp_tag tag;
    union {
    float value;
    uint32_t bytes;
    };
} cp_float_info;

typedef struct cp_long_info {
    cp_tag tag;
    union {
    int64_t value;
    uint64_t bytes;
    };
} cp_long_info;

typedef struct cp_double_info {
    cp_tag tag;
    union {
    double value;
    uint64_t bytes;
    };
} cp_double_info;

typedef struct cp_name_and_type_info {
    cp_tag tag;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
} cp_name_and_type_info;

typedef struct cp_utf8_info {
    cp_tag tag;
    uint16_t length;
    uint8_t bytes[0];
} cp_utf8_info;

cp_info* read_constant(FILE* pClassFile);

#define get_constant(ppConstantPool, constantIndex) ((ppConstantPool)[(constantIndex) - 1]);

void fprint_constant(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pFile);

#endif