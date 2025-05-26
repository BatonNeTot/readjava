#include "method_info.h"

void read_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pClassFile) {
    read_u16(pMethodInfo->accessFlags);
    read_u16(pMethodInfo->nameIndex);
    read_u16(pMethodInfo->descriptorIndex);
    read_u16(pMethodInfo->attributesCount);
    pMethodInfo->attributes = malloc(sizeof(attr_info**) * pMethodInfo->attributesCount);
    for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
        pMethodInfo->attributes[i] = read_attr(ppConstantPool, pClassFile);
    }
}