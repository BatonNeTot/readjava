#include "attr_info.h"


attr_info* read_attr(FILE* pClassFile) {
    uint16_t nameIndex;
    readU16(nameIndex);
    uint32_t length;
    readU32(length);

    attr_info* pInfo = malloc(sizeof(attr_info) + sizeof(uint8_t) * length);
    pInfo->nameIndex = nameIndex;
    pInfo->length = length;
    readBlock(pInfo->info, length);
    return pInfo;
}