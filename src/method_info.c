#include "method_info.h"

#define EOF_ACTION() return false

bool read_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {
    *pMethodInfo = (method_info){0};

    read_u16(pMethodInfo->accessFlags);
    read_u16(pMethodInfo->nameIndex);
    read_u16(pMethodInfo->descriptorIndex);
    read_u16(pMethodInfo->attributesCount);
    if (pMethodInfo->attributesCount > 0) {
        pMethodInfo->attributes = ALLOC(sizeof(attr_info**) * pMethodInfo->attributesCount);
        for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
            pMethodInfo->attributes[i] = read_attr(ppConstantPool, pClassFile, pFilename);
        }

        if (pMethodInfo->attributes[pMethodInfo->attributesCount - 1] == NULL) {
            return false;
        }
    } else {
        pMethodInfo->attributes = NULL;
    }

    return true;
}

void deinit_method(method_info* pMethodInfo) {
    if (pMethodInfo->attributes != NULL) {
        for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
            free_attr(pMethodInfo->attributes[i]);
        }
        FREE(pMethodInfo->attributes);
    }
}

void fprint_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pStream) {
    fprintf(stdout, "method info:\n");

    fprintf(stdout, PD"name        "PD);
    fprint_constant_verbose(pMethodInfo->nameIndex, ppConstantPool, stdout);
    fprintf(stdout, "\n");
    
    fprintf(stdout, PD"descriptor  "PD);
    fprint_constant_verbose(pMethodInfo->descriptorIndex, ppConstantPool, stdout);
    fprintf(stdout, "\n");

    fprintf(pStream, PD"access flags"PD);
    fprint_acesses(pMethodInfo->accessFlags, pStream, ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_SYNCHRONIZED, ACC_NATIVE, ACC_ABSTRACT, ACC_STRICT);
    fprintf(pStream, "\n");
    
    attr_code* pCode = NULL;
    uint16_t nonstandard = 0;

    for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
        attr_info* pAttr = pMethodInfo->attributes[i];
        switch (pAttr->tag) {
            case ATTR_CODE:
                if (pCode == NULL) {
                    pCode = (attr_code*) pAttr;
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

    if (pCode != NULL) {
        fprintf(pStream, PD"Code        "PD);
        for (uint32_t i = 0; i < pCode->codeLength; ++i) {
            fprintf(pStream, "%02"PRIX8" ", pCode->pCode[i]);
        }
        fprintf(pStream, "\n");
    }

    if (nonstandard != 0) {
        fprintf(pStream, PD"non-standard attributes:\n");

        for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
            attr_info* pAttr = pMethodInfo->attributes[i];
            if ((pAttr->tag & nonstandard) == 0) {
                continue;
            }

            fprintf(pStream, PD PD);
            fprint_attr(pAttr, ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}
