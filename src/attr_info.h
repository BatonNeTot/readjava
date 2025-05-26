#ifndef attr_info_h
#define attr_info_h

#include "cp_info.h"

#include "common.h"

typedef struct attr_info {
    uint16_t nameIndex;
    uint32_t length;
    uint8_t info[0];
} attr_info;

typedef struct attr_source_file_info {
    uint16_t nameIndex;
    uint32_t length;
    uint16_t sourceFileIndex;
} attr_source_file_info;

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile);

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pFile);

#endif