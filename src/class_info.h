#ifndef header_h
#define header_h

#include "cp_info.h"
#include "method_info.h"

typedef struct class_info {
    uint16_t minorVersion;
    uint16_t majorVersion;
    
    access_flags accessFlags;
    uint16_t thisClass;
    uint16_t superClass;
    
    uint16_t constantPoolCount;
    uint16_t interfacesCount;
    uint16_t fieldsCount;
    uint16_t methodsCount;
    uint16_t attributesCount;   
    
    cp_tag** ppConstantPool;
    // TODO interfaces
    // TODO fields
    method_info* pMethods;
    attr_info** attributes;
} class_info;

void read_class(class_info* pClassInfo, const char* pFilename);

#endif