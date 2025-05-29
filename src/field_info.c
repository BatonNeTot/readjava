#include "field_info.h"

#define EOF_ACTION() return false

bool read_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {
    read_u16(pFieldInfo->accessFlags);
    read_u16(pFieldInfo->nameIndex);
    read_u16(pFieldInfo->descriptorIndex);
    read_u16(pFieldInfo->attributesCount);
    pFieldInfo->attributes = ALLOC(sizeof(attr_info**) * pFieldInfo->attributesCount);
    for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
        pFieldInfo->attributes[i] = read_attr(ppConstantPool, pClassFile, pFilename);
    }

    if (pFieldInfo->attributes[pFieldInfo->attributesCount - 1] == NULL) {
        return false;
    }

    return true;
}

void deinit_field(field_info* pFieldInfo) {
    if (pFieldInfo->attributes != NULL) {
        for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
            free_attr(pFieldInfo->attributes[i]);
        }
        FREE(pFieldInfo->attributes);
    }
}

void fprint_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pStream) {
    fprintf(pStream, "Field info:\n");

    fprintf(pStream, PD"Name        "PD);
    fprint_constant_short(pFieldInfo->nameIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");
    
    fprintf(pStream, PD"Descriptor  "PD);
    fprint_constant_short(pFieldInfo->descriptorIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"Access flags"PD);
#define ACC_PRINT_VAR pFieldInfo->accessFlags
#define ACC_PRINT_OUTPUT_STREAM pStream
#define ACC_PRINT_PADDING PD
    fprint_acesses_or_none(PUBLIC, PRIVATE, PROTECTED, STATIC, FINAL, VOLATILE, TRANSIENT);
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