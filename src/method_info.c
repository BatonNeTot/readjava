#include "method_info.h"

void read_method(method_info* pMethodInfo, FILE* pClassFile) {
    readU16(pMethodInfo->accessFlags);
    readU16(pMethodInfo->nameIndex);
    readU16(pMethodInfo->descriptorIndex);
    readU16(pMethodInfo->attributesCount);
    pMethodInfo->attributes = malloc(sizeof(attr_info**) * pMethodInfo->attributesCount);
    for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
        pMethodInfo->attributes[i] = read_attr(pClassFile);
    }
}