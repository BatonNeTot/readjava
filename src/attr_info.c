#include "attr_info.h"

#define is_cp_equal_name(name) ((pName)->length == sizeof(name) - 1 && memcmp((pName)->bytes, name, sizeof(name) - 1) == 0)

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile) {
    uint16_t nameIndex;
    read_u16(nameIndex);
    uint32_t length;
    read_u32(length);

    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, nameIndex);

    if (is_cp_equal_name("SourceFile")) {
        attr_source_file* pInfo = malloc(sizeof(attr_source_file));
        pInfo->tag = ATTR_SOURCE_FILE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        read_u16(pInfo->sourceFileIndex);
        return (attr_info*)pInfo;
    }

    if (is_cp_equal_name("Code")) {
        attr_code* pInfo = malloc(sizeof(attr_code));
        pInfo->tag = ATTR_CODE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;;
        read_u16(pInfo->maxStack);
        read_u16(pInfo->maxLocals);

        read_u32(pInfo->codeLength);
        pInfo->pCode = malloc(sizeof(*pInfo->pCode) * pInfo->codeLength);
        read_block(pInfo->pCode, pInfo->codeLength);

        read_u16(pInfo->exceptionTableLength);
        if (pInfo->exceptionTableLength > 0) {
            pInfo->pExceptionTable = malloc(sizeof(*pInfo->pExceptionTable) * pInfo->exceptionTableLength);
            for (uint16_t i = 0; i < pInfo->exceptionTableLength; ++i) {
                read_u16(pInfo->pExceptionTable[i].startPc);
                read_u16(pInfo->pExceptionTable[i].endPc);
                read_u16(pInfo->pExceptionTable[i].handlerPc);
                read_u16(pInfo->pExceptionTable[i].catchType);
            }
        }

        read_u16(pInfo->attributesCount);
        pInfo->attributes = malloc(sizeof(attr_info**) * pInfo->attributesCount);
        for (uint16_t i = 0; i < pInfo->attributesCount; ++i) {
            pInfo->attributes[i] = read_attr(ppConstantPool, pClassFile);
        }

        return (attr_info*)pInfo;
    }

    attr_info* pInfo = malloc(sizeof(attr_info) + sizeof(uint8_t) * length);
    pInfo->tag = ATTR_UNKNOWN;
    pInfo->nameIndex = nameIndex;
    pInfo->length = length;
    read_block(pInfo->info, length);
    return pInfo;
}

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pStream) {
    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, pAttrInfo->nameIndex);

    fprintf(pStream, "%.*s:" PADDING, pName->length, pName->bytes);
    for (uint32_t i = 0; i < pAttrInfo->length; ++i) {
        fprintf(pStream, "%02X" PADDING, pAttrInfo->info[i]);
    }
}