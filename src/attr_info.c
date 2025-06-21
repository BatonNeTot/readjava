#include "attr_info.h"

#define is_cp_equal_name(name) ((pName)->length == sizeof(name) - 1 && memcmp((pName)->bytes, name, sizeof(name) - 1) == 0)

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename) {

#define EOF_ACTION() return NULL
    uint16_t nameIndex;
    read_u16(nameIndex);
    uint32_t length;
    read_u32(length);

#define FREE_ACTION() \
do {\
    free_attr((attr_info*)pInfo);\
    return NULL;\
} while(0)

#undef EOF_ACTION
#define EOF_ACTION() FREE_ACTION()

    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, nameIndex);
    
    if (is_cp_equal_name("ConstantValue")) {
        attr_constant_value* pInfo = ALLOC(sizeof(attr_constant_value));
        pInfo->tag = ATTR_CONSTANT_VALUE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        read_u16(pInfo->constantValueIndex);
        return (attr_info*)pInfo;
    }

    if (is_cp_equal_name("Code")) {
        attr_code* pInfo = ALLOC(sizeof(attr_code));
        *pInfo = (attr_code){0};
        pInfo->tag = ATTR_CODE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;;
        read_u16(pInfo->maxStack);
        read_u16(pInfo->maxLocals);

        read_u32(pInfo->codeLength);
        pInfo->pCode = ALLOC(sizeof(*pInfo->pCode) * pInfo->codeLength);
        read_block(pInfo->pCode, pInfo->codeLength);

        read_u16(pInfo->exceptionTableLength);
        if (pInfo->exceptionTableLength > 0) {
            pInfo->pExceptionTable = ALLOC(sizeof(*pInfo->pExceptionTable) * pInfo->exceptionTableLength);
            for (uint16_t i = 0; i < pInfo->exceptionTableLength; ++i) {
                read_u16(pInfo->pExceptionTable[i].startPc);
                read_u16(pInfo->pExceptionTable[i].endPc);
                read_u16(pInfo->pExceptionTable[i].handlerPc);
                read_u16(pInfo->pExceptionTable[i].catchType);
            }
        } else {
            pInfo->pExceptionTable = NULL;
        }

        read_u16(pInfo->attributesCount);
        if (pInfo->attributesCount > 0) {
            pInfo->attributes = ALLOC(sizeof(attr_info**) * pInfo->attributesCount);
            for (uint16_t i = 0; i < pInfo->attributesCount; ++i) {
                attr_info* pAttr = read_attr(ppConstantPool, pClassFile, pFilename);
                if (pAttr != NULL) {
                    pInfo->attributes[i] = pAttr;
                    continue;
                } 

                while (i > 0) {
                    --i;
                    free_attr(pInfo->attributes[i]);
                }
                FREE(pInfo->attributes);
                FREE(pInfo->pExceptionTable);
                FREE(pInfo->pCode);
                FREE(pInfo);
                return NULL;
            }
        }

        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("Exceptions")) {
        uint16_t numberOfExceptions;
#undef EOF_ACTION
#define EOF_ACTION() return NULL
        read_u16(numberOfExceptions);
#undef EOF_ACTION
#define EOF_ACTION() FREE_ACTION()

        attr_exceptions* pInfo = ALLOC(sizeof(attr_exceptions) + sizeof(uint16_t) * numberOfExceptions);
        pInfo->tag = ATTR_EXCEPTIONS;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        pInfo->numberOfExceptions = numberOfExceptions;
        for (uint16_t i = 0; i < numberOfExceptions; ++i) {
            read_u16(pInfo->exceptionIndexTable[i]);
        }
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("InnerClasses")) {
        uint16_t numberOfClasses;
#undef EOF_ACTION
#define EOF_ACTION() return NULL
        read_u16(numberOfClasses);
#undef EOF_ACTION
#define EOF_ACTION() FREE_ACTION()

        attr_inner_classes* pInfo = ALLOC(sizeof(attr_inner_classes) + sizeof(attr_inner_classes_info) * numberOfClasses);
        pInfo->tag = ATTR_INNER_CLASSES;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        pInfo->numberOfClasses = numberOfClasses;
        for (uint16_t i = 0; i < numberOfClasses; ++i) {
            read_u16(pInfo->classes[i].innerClassInfoIndex);
            read_u16(pInfo->classes[i].outerClassInfoIndex);
            read_u16(pInfo->classes[i].innerNameIndex);
            read_u16(pInfo->classes[i].innerClassAccessFlags);
        }
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("Synthetic")) {
        attr_synthetic* pInfo = ALLOC(sizeof(attr_synthetic));
        pInfo->tag = ATTR_SYNTHETIC;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("SourceFile")) {
        attr_source_file* pInfo = ALLOC(sizeof(attr_source_file));
        pInfo->tag = ATTR_SOURCE_FILE;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        read_u16(pInfo->sourceFileIndex);
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("LineNumberTable")) {
        uint16_t lineNumberTableLength;
#undef EOF_ACTION
#define EOF_ACTION() return NULL
        read_u16(lineNumberTableLength);
#undef EOF_ACTION
#define EOF_ACTION() FREE_ACTION()

        attr_line_number_table* pInfo = ALLOC(sizeof(attr_line_number_table) + sizeof(attr_line_number_table_entry) * lineNumberTableLength);
        pInfo->tag = ATTR_INNER_CLASSES;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        pInfo->lineNumberTableLength = lineNumberTableLength;
        for (uint16_t i = 0; i < lineNumberTableLength; ++i) {
            read_u16(pInfo->lineNumberTable[i].startPc);
            read_u16(pInfo->lineNumberTable[i].lineNumber);
        }
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("LocalVariableTable")) {
        uint16_t localVariableTableLength;
#undef EOF_ACTION
#define EOF_ACTION() return NULL
        read_u16(localVariableTableLength);
#undef EOF_ACTION
#define EOF_ACTION() FREE_ACTION()

        attr_local_variable_table* pInfo = ALLOC(sizeof(attr_local_variable_table) + sizeof(attr_local_variable_table_entry) * localVariableTableLength);
        pInfo->tag = ATTR_INNER_CLASSES;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        pInfo->localVariableTableLength = localVariableTableLength;
        for (uint16_t i = 0; i < localVariableTableLength; ++i) {
            read_u16(pInfo->localVariableTable[i].startPc);
            read_u16(pInfo->localVariableTable[i].length);
            read_u16(pInfo->localVariableTable[i].nameIndex);
            read_u16(pInfo->localVariableTable[i].descriptorIndex);
            read_u16(pInfo->localVariableTable[i].index);
        }
        return (attr_info*)pInfo;
    }
    
    if (is_cp_equal_name("Deprecated")) {
        attr_deprecated* pInfo = ALLOC(sizeof(attr_deprecated));
        pInfo->tag = ATTR_DEPRECATED;
        pInfo->nameIndex = nameIndex;
        pInfo->length = length;
        return (attr_info*)pInfo;
    }

    attr_info* pInfo = ALLOC(sizeof(attr_info) + sizeof(uint8_t) * length);
    pInfo->tag = ATTR_UNKNOWN;
    pInfo->nameIndex = nameIndex;
    pInfo->length = length;
    read_block(pInfo->info, length);
    return pInfo;
}

void free_attr(attr_info* pAttrInfo) {
    if (pAttrInfo->tag == ATTR_CODE) {
        attr_code* pCode = (attr_code*)pAttrInfo;
        if (pCode->attributes != NULL) {
            FREE(pCode->pCode);
            for (uint16_t i = 0; i < pCode->attributesCount; ++i) {
                free_attr(pCode->attributes[i]);
            }
            FREE(pCode->attributes);
        }
    }
    FREE(pAttrInfo);
}

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pStream) {
    cp_utf8_info* pName = (cp_utf8_info*)get_constant(ppConstantPool, pAttrInfo->nameIndex);

    if (pAttrInfo->tag != ATTR_UNKNOWN) {
        fprintf(pStream, "%.*s" PADDING, pName->length, pName->bytes);
        return;
    }

    fprintf(pStream, "%.*s:" PADDING, pName->length, pName->bytes);
    for (uint32_t i = 0; i < pAttrInfo->length; ++i) {
        fprintf(pStream, "%02X" PADDING, pAttrInfo->info[i]);
    }
}