#ifndef field_info_h
#define field_info_h

#include "access_flags.h"
#include "attr_info.h"

#include "common.h"

typedef struct field_info {
    access_flags accessFlags;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t attributesCount;
    attr_info** attributes;
} field_info;

bool read_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename);

void deinit_field(field_info* pFieldInfo);

void fprint_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pStream);

#endif