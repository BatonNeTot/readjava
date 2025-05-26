#include "field_info.h"

void read_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pClassFile) {
    read_u16(pFieldInfo->accessFlags);
    read_u16(pFieldInfo->nameIndex);
    read_u16(pFieldInfo->descriptorIndex);
    read_u16(pFieldInfo->attributesCount);
    pFieldInfo->attributes = malloc(sizeof(attr_info**) * pFieldInfo->attributesCount);
    for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
        pFieldInfo->attributes[i] = read_attr(ppConstantPool, pClassFile);
    }
}