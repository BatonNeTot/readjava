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

void fprint_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pStream) {
    fprintf(pStream, "Field info:\n");

    fprintf(pStream, PD"Name      "PD);
    fprint_constant_short(pFieldInfo->nameIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");
    
    fprintf(pStream, PD"Descriptor"PD);
    fprint_constant_short(pFieldInfo->descriptorIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");

    if (pFieldInfo->attributesCount > 0) {
        fprintf(pStream, PD"Attributes:\n");
        for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
            fprintf(pStream, PD PD);
            fprint_attr(pFieldInfo->attributes[i], ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}