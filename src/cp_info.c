#include "cp_info.h"

#include "common.h"

cp_tag* read_constant(FILE* pClassFile) {
    cp_tag tag;
    readU8(tag);
    
    switch(tag) {
        case CONSTANT_Class: {
            CONSTANT_Class_info* pInfo = malloc(sizeof(CONSTANT_Class_info));
            pInfo->tag = tag;
            readU16(pInfo->nameIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Fieldref: {
            CONSTANT_Fieldref_info* pInfo = malloc(sizeof(CONSTANT_Fieldref_info));
            pInfo->tag = tag;
            readU16(pInfo->classIndex);
            readU16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Methodref: {
            CONSTANT_Methodref_info* pInfo = malloc(sizeof(CONSTANT_Methodref_info));
            pInfo->tag = tag;
            readU16(pInfo->classIndex);
            readU16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_InterfaceMethodref: {
            CONSTANT_InterfaceMethodref_info* pInfo = malloc(sizeof(CONSTANT_InterfaceMethodref_info));
            pInfo->tag = tag;
            readU16(pInfo->classIndex);
            readU16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_String: {
            CONSTANT_String_info* pInfo = malloc(sizeof(CONSTANT_String_info));
            pInfo->tag = tag;
            readU16(pInfo->stringIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Integer: {
            CONSTANT_Integer_info* pInfo = malloc(sizeof(CONSTANT_Integer_info));
            pInfo->tag = tag;
            readU32(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Float: {
            CONSTANT_Float_info* pInfo = malloc(sizeof(CONSTANT_Float_info));
            pInfo->tag = tag;
            readU32(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Long: {
            CONSTANT_Long_info* pInfo = malloc(sizeof(CONSTANT_Long_info));
            pInfo->tag = tag;
            readU64(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Double: {
            CONSTANT_Double_info* pInfo = malloc(sizeof(CONSTANT_Double_info));
            pInfo->tag = tag;
            readU64(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_NameAndType: {
            CONSTANT_NameAndType_info* pInfo = malloc(sizeof(CONSTANT_NameAndType_info));
            pInfo->tag = tag;
            readU16(pInfo->nameIndex);
            readU16(pInfo->descriptorIndex);
            return (cp_tag*)pInfo;
        }
        case CONSTANT_Utf8: {
            uint16_t length;
            readU16(length);
            CONSTANT_Utf8_info* pInfo = malloc(sizeof(CONSTANT_Utf8_info) + sizeof(uint8_t) * length);
            pInfo->tag = tag;
            pInfo->length = length;
            readBlock(pInfo->bytes, length);
            return (cp_tag*)pInfo;
        }
        default: exit(-1);
    }
}