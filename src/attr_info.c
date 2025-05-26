#include "attr_info.h"

#define is_cp_equal_name(name) ((pName)->length == sizeof(name) - 1 && memcmp((pName)->bytes, name, sizeof(name) - 1) == 0)

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile) {
    uint16_t nameIndex;
    read_u16(nameIndex);
    uint32_t length;
    read_u32(length);

    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, nameIndex);

    if (is_cp_equal_name("SourceFile")) {
        attr_source_file_info* pInfo = malloc(sizeof(attr_source_file_info));
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        read_u16(pInfo->sourceFileIndex);
        return (attr_info*)pInfo;
    }

    attr_info* pInfo = malloc(sizeof(attr_info) + sizeof(uint8_t) * length);
    pInfo->nameIndex = nameIndex;
    pInfo->length = length;
    read_block(pInfo->info, length);
    return pInfo;
}

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pFile) {
    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, pAttrInfo->nameIndex);

    if (is_cp_equal_name("SourceFile")) {
        attr_source_file_info* pSourceFile = (attr_source_file_info*)pAttrInfo;
        
        fprintf(pFile, "SourceFile:" PADDING);
        fprint_constant(pSourceFile->sourceFileIndex, ppConstantPool, pFile);
        return;
    }

    fprintf(pFile, "%.*s:" PADDING, pName->length, pName->bytes);
    for (uint32_t i = 0; i < pAttrInfo->length; ++i) {
        fprintf(pFile, "%02X" PADDING, pAttrInfo->info[i]);
    }
}