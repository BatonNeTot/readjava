#ifndef cp_info_h
#define cp_info_h

#include "common.h"

typedef uint8_t cp_tag;

enum __CP_TAG {
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1
};

typedef struct CONSTANT_Class_info {
    cp_tag tag;
    uint16_t nameIndex;
} CONSTANT_Class_info;

typedef struct CONSTANT_Fieldref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} CONSTANT_Fieldref_info;

typedef struct CONSTANT_Methodref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} CONSTANT_Methodref_info;

typedef struct CONSTANT_InterfaceMethodref_info {
    cp_tag tag;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
} CONSTANT_InterfaceMethodref_info;

typedef struct CONSTANT_String_info {
    cp_tag tag;
    uint16_t stringIndex;
} CONSTANT_String_info;

typedef struct CONSTANT_Integer_info {
    cp_tag tag;
    union {
    int32_t value;
    uint32_t bytes;
    };
} CONSTANT_Integer_info;

typedef struct CONSTANT_Float_info {
    cp_tag tag;
    union {
    float value;
    uint32_t bytes;
    };
} CONSTANT_Float_info;

typedef struct CONSTANT_Long_info {
    cp_tag tag;
    union {
    int64_t value;
    uint64_t bytes;
    };
} CONSTANT_Long_info;

typedef struct CONSTANT_Double_info {
    cp_tag tag;
    union {
    double value;
    uint64_t bytes;
    };
} CONSTANT_Double_info;

typedef struct CONSTANT_NameAndType_info {
    cp_tag tag;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
} CONSTANT_NameAndType_info;

typedef struct CONSTANT_Utf8_info {
    cp_tag tag;
    uint16_t length;
    uint8_t bytes[0];
} CONSTANT_Utf8_info;

cp_tag* read_constant(FILE* pClassFile);

#endif