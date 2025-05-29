#ifndef method_info_h
#define method_info_h

#include "access_flags.h"
#include "attr_info.h"

#include "common.h"

typedef struct method_info {
    access_flags accessFlags;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t attributesCount;
    attr_info** attributes;
} method_info;

bool read_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename);

void deinit_method(method_info* pMethodInfo);

void fprint_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pStream);

#endif