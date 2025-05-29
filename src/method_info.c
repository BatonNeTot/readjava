#include "method_info.h"

#define EOF_ACTION() return false

bool read_method(method_info* pMethodInfo, cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {
    *pMethodInfo = (method_info){0};

    read_u16(pMethodInfo->accessFlags);
    read_u16(pMethodInfo->nameIndex);
    read_u16(pMethodInfo->descriptorIndex);
    read_u16(pMethodInfo->attributesCount);
    pMethodInfo->attributes = ALLOC(sizeof(attr_info**) * pMethodInfo->attributesCount);
    for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
        pMethodInfo->attributes[i] = read_attr(ppConstantPool, pClassFile, pFilename);
    }

    if (pMethodInfo->attributes[pMethodInfo->attributesCount - 1] == NULL) {
        return false;
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
    fprintf(stdout, "Method info:\n");

    fprintf(stdout, PD"Name        "PD);
    fprint_constant_short(pMethodInfo->nameIndex, ppConstantPool, stdout);
    fprintf(stdout, "\n");
    
    fprintf(stdout, PD"Descriptor  "PD);
    fprint_constant_short(pMethodInfo->descriptorIndex, ppConstantPool, stdout);
    fprintf(stdout, "\n");

    fprintf(pStream, PD"Access flags"PD);
#define ACC_PRINT_VAR pMethodInfo->accessFlags
#define ACC_PRINT_OUTPUT_STREAM pStream
#define ACC_PRINT_PADDING PD
    fprint_acesses_or_none(PUBLIC, PRIVATE, PROTECTED, STATIC, FINAL, SYNCHRONIZED, NATIVE, ABSTRACT, STRICT);
    fprintf(pStream, "\n");
    
    attr_code* pCode = NULL;
    uint16_t unsupported = 0;

    for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
        attr_info* pAttr = pMethodInfo->attributes[i];
        switch (pAttr->tag) {
            case ATTR_CODE:
                if (pCode == NULL) {
                    pCode = (attr_code*) pAttr;
                }
                break;
            default:
                unsupported |= pAttr->tag;
        }
    }

    if (pCode != NULL) {
        fprintf(pStream, PD"Code        "PD);
        for (uint32_t i = 0; i < pCode->codeLength; ++i) {
            fprintf(pStream, "%02"PRIX8" ", pCode->pCode[i]);
        }
        fprintf(pStream, "\n");
    } else {
        fprintf(pStream, PD"No code\n");
    }

    if (unsupported != 0) {
        fprintf(pStream, PD"Unsupported attributes\n");

        for (uint16_t i = 0; i < pMethodInfo->attributesCount; ++i) {
            attr_info* pAttr = pMethodInfo->attributes[i];
            if ((pAttr->tag & unsupported) == 0) {
                continue;
            }

            fprintf(pStream, PD PD);
            fprint_attr(pAttr, ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}
