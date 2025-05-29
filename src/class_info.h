#ifndef header_h
#define header_h

#include "access_flags.h"
#include "cp_info.h"
#include "field_info.h"
#include "method_info.h"

typedef struct class_info {
    uint16_t minorVersion;
    uint16_t majorVersion;
    
    access_flags accessFlags;
    uint16_t thisClassIndex;
    uint16_t superClassIndex;
    
    uint16_t constantPoolCount;
    uint16_t interfacesCount;
    uint16_t fieldsCount;
    uint16_t methodsCount;
    uint16_t attributesCount;   
    
    cp_info** ppConstantPool;
    uint16_t* pInterfaces;
    field_info* pFields;
    method_info* pMethods;
    attr_info** attributes;
} class_info;

bool read_class(class_info* pClassInfo, const char* pFilename);

void deinit_class(class_info* pClassInfo);

void fprint_class(class_info* pClassInfo, FILE* pStream);

#endif