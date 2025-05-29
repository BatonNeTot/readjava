#include "class_info.h"

#include "common.h"

const uint8_t aJavaMagic[4] = { 0xCA, 0xFE, 0xBA, 0xBE };

#define javaMagicLength (sizeof(aJavaMagic) / sizeof(*(aJavaMagic)))

bool read_class(class_info* pClassInfo, const char* pFilename) {
    FILE* pClassFile = fopen(pFilename, "rb");
    if (pClassFile == NULL) {
        fprintf(stderr, "Found no file named \"%s\".\n", pFilename);
        exit(-1);
    }

    uint8_t buffer[sizeof(aJavaMagic)];
    size_t magicReadCount = fread(buffer, 1, javaMagicLength, pClassFile);
    if (magicReadCount != javaMagicLength || memcmp(buffer, aJavaMagic, javaMagicLength)) {
        fprintf(stderr, "File %s is not supported java class.\n", pFilename);
        exit(-1);
    }


    read_u16(pClassInfo->minorVersion);
    read_u16(pClassInfo->majorVersion);

    read_u16(pClassInfo->constantPoolCount);
    pClassInfo->ppConstantPool = malloc(sizeof(*pClassInfo->ppConstantPool) * (pClassInfo->constantPoolCount - 1));
    for (uint16_t i = 1; i < pClassInfo->constantPoolCount; ++i) {
        pClassInfo->ppConstantPool[i - 1] = read_constant(pClassFile);
    }

    read_u16(pClassInfo->accessFlags);
    read_u16(pClassInfo->thisClassIndex);
    read_u16(pClassInfo->superClassIndex);


    read_u16(pClassInfo->interfacesCount);
    pClassInfo->pInterfaces = malloc(sizeof(*pClassInfo->pInterfaces) * (pClassInfo->interfacesCount));
    for (uint16_t i = 0; i < pClassInfo->interfacesCount; ++i) {
        read_u16(pClassInfo->pInterfaces[i]);
    } 
    
    read_u16(pClassInfo->fieldsCount);
    pClassInfo->pFields = malloc(sizeof(*pClassInfo->pFields) * (pClassInfo->fieldsCount));
    for (uint16_t i = 0; i < pClassInfo->fieldsCount; ++i) {
        read_field(pClassInfo->pFields + i, pClassInfo->ppConstantPool, pClassFile);
    } 

    read_u16(pClassInfo->methodsCount);
    pClassInfo->pMethods = malloc(sizeof(*pClassInfo->pMethods) * (pClassInfo->methodsCount));
    for (uint16_t i = 0; i < pClassInfo->methodsCount; ++i) {
        read_method(pClassInfo->pMethods + i, pClassInfo->ppConstantPool, pClassFile);
    } 

    read_u16(pClassInfo->attributesCount);
    pClassInfo->attributes = malloc(sizeof(attr_info**) * pClassInfo->attributesCount);
    for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
        pClassInfo->attributes[i] = read_attr(pClassInfo->ppConstantPool, pClassFile);
    }

    fclose(pClassFile);
}

void fprint_class(class_info* pClassInfo, FILE* pStream) {
    fprintf(pStream, "Class info:\n");
    fprintf(pStream, PD"Version     "PD"%"PRIu16".%"PRIu16"\n", pClassInfo->majorVersion, pClassInfo->minorVersion);
    
    fprintf(pStream, PD"Class       "PD);
    fprint_constant_short(pClassInfo->thisClassIndex, pClassInfo->ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"Super       "PD);
    fprint_constant_short(pClassInfo->superClassIndex, pClassInfo->ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"Access flags"PD);
    fprint_access(pStream, pClassInfo->accessFlags, "", PUBLIC, PD);
    fprint_access(pStream, pClassInfo->accessFlags, "", FINAL, PD);
    fprint_access(pStream, pClassInfo->accessFlags, "", SUPER, PD);
    fprint_access(pStream, pClassInfo->accessFlags, "", INTERFACE, PD);
    fprint_access(pStream, pClassInfo->accessFlags, "", ABSTRACT, PD);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"Interfaces  "PD);
    if (pClassInfo->interfacesCount == 0) {
        fprintf(pStream, "---");
    } else
    for (uint16_t i = 0; i < pClassInfo->interfacesCount; ++i) {
        fprint_constant_short(pClassInfo->pInterfaces[i], pClassInfo->ppConstantPool, pStream);
        fprintf(pStream, PD);
    }
    fprintf(pStream, "\n");

    fprintf(pStream, PD"Fields      "PD"%"PRIu16"\n", pClassInfo->fieldsCount);
    fprintf(pStream, PD"Methods     "PD"%"PRIu16"\n", pClassInfo->methodsCount);

    attr_source_file* pSourceFile = NULL;
    uint16_t unsupported = 0;

    for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
        attr_info* pAttr = pClassInfo->attributes[i];
        switch (pAttr->tag) {
            case ATTR_SOURCE_FILE:
                if (pSourceFile == NULL) {
                    pSourceFile = (attr_source_file*) pAttr;
                }
                break;
            default:
                unsupported |= pAttr->tag;
        }
    }

    if (pSourceFile != NULL) {
        fprintf(pStream, PD"Source file "PD);
        fprint_constant_short(pSourceFile->sourceFileIndex, pClassInfo->ppConstantPool, pStream);
        fprintf(pStream, "\n");
    } else {
        fprintf(pStream, PD"No source file\n");
    }

    if (unsupported != 0) {
        fprintf(pStream, PD"Unsupported attributes\n");

        for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
            attr_info* pAttr = pClassInfo->attributes[i];
            if ((pAttr->tag & unsupported) == 0) {
                continue;
            }

            fprintf(pStream, PD PD);
            fprint_attr(pAttr, pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}
