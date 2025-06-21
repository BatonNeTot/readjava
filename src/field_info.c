#include "field_info.h"

#define EOF_ACTION() return false

bool read_field(field_info* pFieldInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {
    read_u16(pFieldInfo->accessFlags);
    read_u16(pFieldInfo->nameIndex);
    read_u16(pFieldInfo->descriptorIndex);
    read_u16(pFieldInfo->attributesCount);
    if (pFieldInfo->attributesCount > 0) {
        pFieldInfo->attributes = ALLOC(sizeof(attr_info**) * pFieldInfo->attributesCount);
        for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
            pFieldInfo->attributes[i] = read_attr(ppConstantPool, pClassFile, pFilename);
        }

        if (pFieldInfo->attributes[pFieldInfo->attributesCount - 1] == NULL) {
            return false;
        }
    } else {
        pFieldInfo->attributes = NULL;
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
    fprintf(pStream, "field info:\n");

    fprintf(pStream, PD"name         "PD);
    fprint_constant_verbose(pFieldInfo->nameIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");
    
    fprintf(pStream, PD"descriptor   "PD);
    fprint_constant_verbose(pFieldInfo->descriptorIndex, ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"access flags "PD);
    fprint_acesses(pFieldInfo->accessFlags, pStream, ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_VOLATILE, ACC_TRANSIENT);
    fprintf(pStream, "\n");

    attr_constant_value* pConstantValue = NULL;
    uint16_t nonstandard = 0;

    for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
        attr_info* pAttr = pFieldInfo->attributes[i];
        switch (pAttr->tag) {
            case ATTR_CONSTANT_VALUE:
                if (pConstantValue == NULL) {
                    pConstantValue = (attr_constant_value*) pAttr;
                }
                break;
            case ATTR_SYNTHETIC:
                fprintf(pStream, PD"Synthetic\n");
                break;
            case ATTR_DEPRECATED:
                fprintf(pStream, PD"Deprecated\n");
                break;
            default:
                nonstandard |= pAttr->tag;
        }
    }
    
    if (pConstantValue != NULL) {
        fprintf(pStream, PD"ConstantValue"PD);
        fprint_constant_verbose(pConstantValue->constantValueIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
    }

    if (nonstandard != 0) {
        fprintf(pStream, PD"non-standard attributes:\n");

        for (uint16_t i = 0; i < pFieldInfo->attributesCount; ++i) {
            attr_info* pAttr = pFieldInfo->attributes[i];
            if ((pAttr->tag & nonstandard) == 0) {
                continue;
            }

            fprintf(pStream, PD PD);
            fprint_attr(pAttr, ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}