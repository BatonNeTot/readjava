#include "class_info.h"

#include "common.h"

const uint8_t aJavaMagic[4] = { 0xCA, 0xFE, 0xBA, 0xBE };

#define javaMagicLength (sizeof(aJavaMagic) / sizeof(*(aJavaMagic)))

bool read_class(class_info* pClassInfo, const char* pFilename) {
    FILE* pClassFile = fopen(pFilename, "rb");
    if (pClassFile == NULL) {
        fprintf(stderr, "readjava: Error: '%s': No such file\n", pFilename);
        return false;
    }

    uint8_t buffer[sizeof(aJavaMagic)];
    size_t magicReadCount = fread(buffer, 1, javaMagicLength, pClassFile);
    if (magicReadCount != javaMagicLength || memcmp(buffer, aJavaMagic, javaMagicLength)) {
        fprintf(stderr, "readjava: Error: '%s': Not a java class - it has the wrong magic bytes at the start\n", pFilename);
        return false;
    }

#undef EOF_ACTION
#define EOF_ACTION() \
do {\
    fclose(pClassFile);\
    deinit_class(pClassInfo);\
    return false;\
} while(0)

    bool success = true;

    read_u16(pClassInfo->minorVersion);
    read_u16(pClassInfo->majorVersion);

    read_u16(pClassInfo->constantPoolCount);
    pClassInfo->ppConstantPool = ALLOC(sizeof(*pClassInfo->ppConstantPool) * (pClassInfo->constantPoolCount - 1));
    for (uint16_t i = 1; i < pClassInfo->constantPoolCount; ++i) {
        cp_info* pCpInfo = read_constant(pClassFile, pFilename);
        pClassInfo->ppConstantPool[i - 1] = pCpInfo;
        success &= (pCpInfo != NULL);
    }

    if (!success) {
        fclose(pClassFile);
        return false;
    }

    read_u16(pClassInfo->accessFlags);
    read_u16(pClassInfo->thisClassIndex);
    read_u16(pClassInfo->superClassIndex);


    read_u16(pClassInfo->interfacesCount);
    pClassInfo->pInterfaces = ALLOC(sizeof(*pClassInfo->pInterfaces) * (pClassInfo->interfacesCount));
    for (uint16_t i = 0; i < pClassInfo->interfacesCount; ++i) {
        read_u16(pClassInfo->pInterfaces[i]);
    } 
    
    read_u16(pClassInfo->fieldsCount);
    pClassInfo->pFields = ALLOC(sizeof(*pClassInfo->pFields) * (pClassInfo->fieldsCount));
    for (uint16_t i = 0; i < pClassInfo->fieldsCount; ++i) {
        success &= read_field(pClassInfo->pFields + i, pClassInfo->ppConstantPool, pClassFile, pFilename);
    } 

    if (!success) {
        fclose(pClassFile);
        return false;
    }

    read_u16(pClassInfo->methodsCount);
    pClassInfo->pMethods = ALLOC(sizeof(*pClassInfo->pMethods) * (pClassInfo->methodsCount));
    for (uint16_t i = 0; i < pClassInfo->methodsCount; ++i) {
        success &= read_method(pClassInfo->pMethods + i, pClassInfo->ppConstantPool, pClassFile, pFilename);
    } 

    if (!success) {
        fclose(pClassFile);
        return false;
    }

    read_u16(pClassInfo->attributesCount);
    pClassInfo->attributes = ALLOC(sizeof(attr_info**) * pClassInfo->attributesCount);
    for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
        attr_info* pAttr = read_attr(pClassInfo->ppConstantPool, pClassFile, pFilename);
        pClassInfo->attributes[i] = pAttr;
        success &= (pAttr != NULL);
    }

    fclose(pClassFile);
    return success;
}

void deinit_class(class_info* pClassInfo) {
    if (pClassInfo->attributes != NULL) {
        for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
            free_attr(pClassInfo->attributes[i]);
        }
        FREE(pClassInfo->attributes);
    }
    if (pClassInfo->pMethods != NULL) {
        for (uint16_t i = 0; i < pClassInfo->methodsCount; ++i) {
            deinit_method(pClassInfo->pMethods + i);
        }
        FREE(pClassInfo->pMethods);
    }
    if (pClassInfo->pFields != NULL) {
        for (uint16_t i = 0; i < pClassInfo->fieldsCount; ++i) {
            deinit_field(pClassInfo->pFields + i);
        }
        FREE(pClassInfo->pFields);
    }
    FREE(pClassInfo->pInterfaces);
    if (pClassInfo->ppConstantPool != NULL) {
        for (uint16_t i = 0; i < pClassInfo->constantPoolCount; ++i) {
            FREE(pClassInfo->ppConstantPool[i]);
        }
        FREE(pClassInfo->ppConstantPool);
    }
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
#define ACC_PRINT_VAR pClassInfo->accessFlags
#define ACC_PRINT_OUTPUT_STREAM pStream
#define ACC_PRINT_PADDING PD
    fprint_acesses_or_none(PUBLIC, FINAL, SUPER, INTERFACE, ABSTRACT);
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
