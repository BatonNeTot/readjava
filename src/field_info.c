#include "field_info.h"

void read_field(field_info* pFieldInfo, FILE* pClassFile) {
    readU16(pFieldInfo->accessFlags);
    readU16(pFieldInfo->nameIndex);
    readU16(pFieldInfo->descriptorIndex);
    readU16(pFieldInfo->attributesCount);
    pFieldInfo->attributes = malloc(sizeof(attr_info**) * pFieldInfo->attributesCount);
    for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
        pFieldInfo->attributes[i] = read_attr(pClassFile);
    }
}