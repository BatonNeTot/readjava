#include "attr_info.h"

#define is_cp_equal_name(name) ((pName)->length == sizeof(name) - 1 && memcmp((pName)->bytes, name, sizeof(name) - 1) == 0)

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {

#define EOF_ACTION() return NULL
    uint16_t nameIndex;
    read_u16(nameIndex);
    uint32_t length;
    read_u32(length);

#undef EOF_ACTION
#define EOF_ACTION() \
do {\
    free_attr((attr_info*)pInfo);\
    return NULL;\
} while(0)

    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, nameIndex);

    if (is_cp_equal_name("SourceFile")) {
        attr_source_file* pInfo = ALLOC(sizeof(attr_source_file));
        pInfo->tag = ATTR_SOURCE_FILE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        read_u16(pInfo->sourceFileIndex);
        return (attr_info*)pInfo;
    }

    if (is_cp_equal_name("Code")) {
        attr_code* pInfo = ALLOC(sizeof(attr_code));
        *pInfo = (attr_code){0};
        pInfo->tag = ATTR_CODE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;;
        read_u16(pInfo->maxStack);
        read_u16(pInfo->maxLocals);

        read_u32(pInfo->codeLength);
        pInfo->pCode = ALLOC(sizeof(*pInfo->pCode) * pInfo->codeLength);
        read_block(pInfo->pCode, pInfo->codeLength);

        read_u16(pInfo->exceptionTableLength);
        if (pInfo->exceptionTableLength > 0) {
            pInfo->pExceptionTable = ALLOC(sizeof(*pInfo->pExceptionTable) * pInfo->exceptionTableLength);
            for (uint16_t i = 0; i < pInfo->exceptionTableLength; ++i) {
                read_u16(pInfo->pExceptionTable[i].startPc);
                read_u16(pInfo->pExceptionTable[i].endPc);
                read_u16(pInfo->pExceptionTable[i].handlerPc);
                read_u16(pInfo->pExceptionTable[i].catchType);
            }
        } else {
            pInfo->pExceptionTable = NULL;
        }

        read_u16(pInfo->attributesCount);
        pInfo->attributes = ALLOC(sizeof(attr_info**) * pInfo->attributesCount);
        for (uint16_t i = 0; i < pInfo->attributesCount; ++i) {
            attr_info* pAttr = read_attr(ppConstantPool, pClassFile, pFilename);
            if (pAttr != NULL) {
                pInfo->attributes[i] = pAttr;
                continue;
            } 

            while (i > 0) {
                --i;
                free_attr(pInfo->attributes[i]);
            }
            FREE(pInfo->attributes);
            FREE(pInfo->pExceptionTable);
            FREE(pInfo->pCode);
            FREE(pInfo);
            return NULL;
        }

        return (attr_info*)pInfo;
    }

    attr_info* pInfo = ALLOC(sizeof(attr_info) + sizeof(uint8_t) * length);
    pInfo->tag = ATTR_UNKNOWN;
    pInfo->nameIndex = nameIndex;
    pInfo->length = length;
    read_block(pInfo->info, length);
    return pInfo;
}

void free_attr(attr_info* pAttrInfo) {
    switch (pAttrInfo->tag) {
        case ATTR_UNKNOWN:
        case ATTR_CONSTANT:
        case ATTR_SOURCE_FILE: {
            FREE(pAttrInfo);
            break;
        }
        case ATTR_CODE: {
            attr_code* pCode = (attr_code*)pAttrInfo;
            if (pCode->attributes != NULL) {
                FREE(pCode->pCode);
                for (uint16_t i = 0; i < pCode->attributesCount; ++i) {
                    free_attr(pCode->attributes[i]);
                }
                FREE(pCode->attributes);
            }
            FREE(pCode);
            break;
        }
    }
}

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pStream) {
    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, pAttrInfo->nameIndex);

    fprintf(pStream, "%.*s:" PADDING, pName->length, pName->bytes);
    for (uint32_t i = 0; i < pAttrInfo->length; ++i) {
        fprintf(pStream, "%02X" PADDING, pAttrInfo->info[i]);
    }
}