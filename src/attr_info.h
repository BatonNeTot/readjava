#ifndef attr_info_h
#define attr_info_h

#include "common.h"

typedef struct attr_info {
    uint16_t nameIndex;
    uint32_t length;
    uint8_t info[0];
} attr_info;

attr_info* read_attr(FILE* pClassFile);

#endif