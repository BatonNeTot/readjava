#ifndef method_info_h
#define method_info_h

#include "attr_info.h"

#include "common.h"

typedef struct method_info {
    access_flags accessFlags;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t attributesCount;
    attr_info** attributes;
} method_info;

void read_method(method_info* pMethodInfo, FILE* pClassFile);

#endif