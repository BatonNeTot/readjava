#ifndef attr_info_h
#define attr_info_h

#include "cp_info.h"

#include "common.h"

typedef uint16_t attr_tag;

enum __ATTR_TAG {
    ATTR_UNKNOWN        = 0x0001,
    ATTR_CONSTANT       = 0x0002,
    ATTR_SOURCE_FILE    = 0x0004,
    ATTR_CODE           = 0x0008,
};

typedef struct attr_info {
    attr_tag tag;
    uint16_t nameIndex;
    uint32_t length;
    uint8_t info[0];
} attr_info;

typedef struct attr_code_exception {
    uint16_t startPc;
    uint16_t endPc;
    uint16_t  handlerPc;
    uint16_t  catchType;
} attr_code_exception;

typedef struct attr_code {
    attr_tag tag;
    uint16_t nameIndex;
    uint32_t length;
    uint16_t maxStack;
    uint16_t maxLocals;
    uint16_t exceptionTableLength;
    uint16_t attributesCount;
    uint32_t codeLength;
    attr_info** attributes;
    uint8_t* pCode;
    attr_code_exception* pExceptionTable;
} attr_code;

typedef struct attr_source_file {
    attr_tag tag;
    uint16_t nameIndex;
    uint32_t length;
    uint16_t sourceFileIndex;
} attr_source_file;

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename);

void free_attr(attr_info* pAttrInfo);

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pStream);

#endif