#include "class_info.h"

#include "common.h"

const uint8_t aJavaMagic[4] = { 0xCA, 0xFE, 0xBA, 0xBE };

#define javaMagicLength (sizeof(aJavaMagic) / sizeof(*(aJavaMagic)))

bool read_class(class_info* pClassInfo, const char* pFilename) {
    *pClassInfo = (class_info){0};

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
    if (pClassInfo->interfacesCount > 0) {
        pClassInfo->pInterfaces = ALLOC(sizeof(*pClassInfo->pInterfaces) * (pClassInfo->interfacesCount));
        for (uint16_t i = 0; i < pClassInfo->interfacesCount; ++i) {
            read_u16(pClassInfo->pInterfaces[i]);
        }
    } 
    
    read_u16(pClassInfo->fieldsCount);
    if (pClassInfo->fieldsCount > 0) {
        pClassInfo->pFields = ALLOC(sizeof(*pClassInfo->pFields) * (pClassInfo->fieldsCount));
        for (uint16_t i = 0; i < pClassInfo->fieldsCount; ++i) {
            success &= read_field(pClassInfo->pFields + i, pClassInfo->ppConstantPool, pClassFile, pFilename);
        }
    }

    if (!success) {
        fclose(pClassFile);
        return false;
    }

    read_u16(pClassInfo->methodsCount);
    if (pClassInfo->methodsCount > 0) {
        pClassInfo->pMethods = ALLOC(sizeof(*pClassInfo->pMethods) * (pClassInfo->methodsCount));
        for (uint16_t i = 0; i < pClassInfo->methodsCount; ++i) {
            success &= read_method(pClassInfo->pMethods + i, pClassInfo->ppConstantPool, pClassFile, pFilename);
        }
    }

    if (!success) {
        fclose(pClassFile);
        return false;
    }

    read_u16(pClassInfo->attributesCount);
    if (pClassInfo->attributesCount > 0) {
        pClassInfo->attributes = ALLOC(sizeof(attr_info**) * pClassInfo->attributesCount);
        for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
            attr_info* pAttr = read_attr(pClassInfo->ppConstantPool, pClassFile, pFilename);
            pClassInfo->attributes[i] = pAttr;
            success &= (pAttr != NULL);
        }
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
    fprintf(pStream, "class info:\n");
    fprintf(pStream, PD"version             "PD"%"PRIu16".%"PRIu16"\n", pClassInfo->majorVersion, pClassInfo->minorVersion);
    
    fprintf(pStream, PD"class               "PD);
    fprint_constant_verbose(pClassInfo->thisClassIndex, pClassInfo->ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"super               "PD);
    fprint_constant_verbose(pClassInfo->superClassIndex, pClassInfo->ppConstantPool, pStream);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"access flags        "PD);
    fprint_acesses(pClassInfo->accessFlags, pStream, ACC_PUBLIC, ACC_FINAL, ACC_SUPER, ACC_INTERFACE, ACC_ABSTRACT);
    fprintf(pStream, "\n");

    fprintf(pStream, PD"interfaces");
    if (pClassInfo->interfacesCount == 0) {
        fprintf(pStream, "          "PD"none\n");
    } else {
        fprintf(pStream, ":\n");
        for (uint16_t i = 0; i < pClassInfo->interfacesCount; ++i) {
            fprintf(pStream, PD PD"                  "PD);
            fprint_constant_verbose(pClassInfo->pInterfaces[i], pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }

    fprintf(pStream, PD"fields        "PD"%"PRIu16"\n", pClassInfo->fieldsCount);
    fprintf(pStream, PD"methods       "PD"%"PRIu16"\n", pClassInfo->methodsCount);

    attr_inner_classes* pInnerClasses = NULL;
    attr_source_file* pSourceFile = NULL;
    uint16_t nonstandard = 0;

    for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
        attr_info* pAttr = pClassInfo->attributes[i];
        switch (pAttr->tag) {
            case ATTR_INNER_CLASSES:
                if (pInnerClasses == NULL) {
                    pInnerClasses = (attr_inner_classes*) pAttr;
                }
                break;
            case ATTR_SOURCE_FILE:
                if (pSourceFile == NULL) {
                    pSourceFile = (attr_source_file*) pAttr;
                }
                break;
            case ATTR_SYNTHETIC:
                fprintf(pStream, PD"Synthetic\n");
                break;
            case ATTR_DEPRECATED:
                fprintf(pStream, PD"Deprecated\n");
                break;
            default:
                nonstandard |= pAttr->tag;
        }
    }

    if (pInnerClasses != NULL) {
        fprintf(pStream, PD"inner classes:\n");

        for (uint16_t i = 0; i < pInnerClasses->numberOfClasses; ++i) {
            attr_inner_classes_info* pInfo = pInnerClasses->classes + i;

            fprintf(pStream, PD PD "inner class "PD);
            fprint_constant_verbose(pInfo->innerClassInfoIndex, pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");

            fprintf(pStream, PD PD "outer class "PD);
            fprint_constant_verbose(pInfo->outerClassInfoIndex, pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");

            fprintf(pStream, PD PD "name        "PD);
            fprint_constant_verbose(pInfo->innerNameIndex, pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");
            
            fprintf(pStream, PD PD "access flags"PD);
            fprint_acesses(pInfo->innerClassAccessFlags, pStream, ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_INTERFACE, ACC_ABSTRACT);
            fprintf(pStream, "\n");
        }
    }

    if (pSourceFile != NULL) {
        fprintf(pStream, PD"SourceFile          "PD);
        fprint_constant_verbose(pSourceFile->sourceFileIndex, pClassInfo->ppConstantPool, pStream);
        fprintf(pStream, "\n");
    }

    if (nonstandard != 0) {
        fprintf(pStream, PD"non-standard attributes:\n");

        for (uint16_t i = 0; i < pClassInfo->attributesCount; ++i) {
            attr_info* pAttr = pClassInfo->attributes[i];
            if ((pAttr->tag & nonstandard) == 0) {
                continue;
            }

            fprintf(pStream, PD PD);
            fprint_attr(pAttr, pClassInfo->ppConstantPool, pStream);
            fprintf(pStream, "\n");
        }
    }
}
