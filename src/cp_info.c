#include "cp_info.h"

#include "common.h"

cp_info* read_constant(FILE* pClassFile, const char* pFilename) {

#define FREE_AND_RETURN 

#define EOF_ACTION() return NULL
    cp_tag tag;
    read_u8(tag);

    switch(tag) {
        case CP_UTF8: {
            uint16_t length;
            read_u16(length);

#undef EOF_ACTION
#define EOF_ACTION() \
do {\
    FREE(pInfo);\
    return NULL;\
} while(0)
            cp_utf8_info* pInfo = ALLOC(sizeof(cp_utf8_info) + sizeof(uint8_t) * length);
            pInfo->tag = tag;
            pInfo->length = length;
            read_block(pInfo->bytes, length);
            return (cp_info*)pInfo;
        }
        case CP_CLASS: {
            cp_class_info* pInfo = ALLOC(sizeof(cp_class_info));
            pInfo->tag = tag;
            read_u16(pInfo->nameIndex);
            return (cp_info*)pInfo;
        }
        case CP_FIELDREF: {
            cp_fieldref_info* pInfo = ALLOC(sizeof(cp_fieldref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_info*)pInfo;
        }
        case CP_METHODREF: {
            cp_methodref_info* pInfo = ALLOC(sizeof(cp_methodref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_info*)pInfo;
        }
        case CP_IMETHODREF: {
            cp_imethodref_info* pInfo = ALLOC(sizeof(cp_imethodref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_info*)pInfo;
        }
        case CP_STRING: {
            cp_string_info* pInfo = ALLOC(sizeof(cp_string_info));
            pInfo->tag = tag;
            read_u16(pInfo->stringIndex);
            return (cp_info*)pInfo;
        }
        case CP_INTEGER: {
            cp_integer_info* pInfo = ALLOC(sizeof(cp_integer_info));
            pInfo->tag = tag;
            read_u32(pInfo->bytes);
            return (cp_info*)pInfo;
        }
        case CP_FLOAT: {
            cp_float_info* pInfo = ALLOC(sizeof(cp_float_info));
            pInfo->tag = tag;
            read_u32(pInfo->bytes);
            return (cp_info*)pInfo;
        }
        case CP_LONG: {
            cp_long_info* pInfo = ALLOC(sizeof(cp_long_info));
            pInfo->tag = tag;
            read_u64(pInfo->bytes);
            return (cp_info*)pInfo;
        }
        case CP_DOUBLE: {
            cp_double_info* pInfo = ALLOC(sizeof(cp_double_info));
            pInfo->tag = tag;
            read_u64(pInfo->bytes);
            return (cp_info*)pInfo;
        }
        case CP_NAME_AND_TYPE: {
            cp_name_and_type_info* pInfo = ALLOC(sizeof(cp_name_and_type_info));
            pInfo->tag = tag;
            read_u16(pInfo->nameIndex);
            read_u16(pInfo->descriptorIndex);
            return (cp_info*)pInfo;
        }
        default: 
            fprintf(stderr, "readjava: Error: '%s': unknown constant tag %02X.\n", pFilename, tag);
            return NULL;
    }
}

void fprint_constant(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pStream) {
    if (constantIndex == 0) {
        // TODO special case
        return;
    }

    cp_info* pBaseCpInfo = get_constant(ppConstantPool, constantIndex);
    switch (*pBaseCpInfo) {
    case CP_CLASS: {
        cp_class_info* pCpInfo = (cp_class_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->nameIndex, ppConstantPool, pStream);
        break;
    }
    case CP_FIELDREF: {
        cp_fieldref_info* pCpInfo = (cp_fieldref_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->classIndex, ppConstantPool, pStream);
        fprintf(pStream, ".");
        fprint_constant(pCpInfo->nameAndTypeIndex, ppConstantPool, pStream);
        break;
    }
    case CP_METHODREF: {
        cp_methodref_info* pCpInfo = (cp_methodref_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->classIndex, ppConstantPool, pStream);
        fprintf(pStream, ".");
        fprint_constant(pCpInfo->nameAndTypeIndex, ppConstantPool, pStream);
        break;
    }
    case CP_IMETHODREF: {
        cp_imethodref_info* pCpInfo = (cp_imethodref_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->classIndex, ppConstantPool, pStream);
        fprintf(pStream, ".");
        fprint_constant(pCpInfo->nameAndTypeIndex, ppConstantPool, pStream);
        break;
    }
    case CP_STRING: {
        cp_string_info* pCpInfo = (cp_string_info*)pBaseCpInfo;

        fprintf(pStream, "\"");
        fprint_constant(pCpInfo->stringIndex, ppConstantPool, pStream);
        fprintf(pStream, "\"");
        break;
    }
    case CP_INTEGER: {
        cp_integer_info* pCpInfo = (cp_integer_info*)pBaseCpInfo;

        fprintf(pStream, "%"PRId32, pCpInfo->value);
        break;
    }
    case CP_FLOAT: {
        cp_float_info* pCpInfo = (cp_float_info*)pBaseCpInfo;

        fprintf(pStream, "%f", pCpInfo->value);
        break;
    }
    case CP_LONG: {
        cp_long_info* pCpInfo = (cp_long_info*)pBaseCpInfo;

        fprintf(pStream, "%"PRId64, pCpInfo->value);
        break;
    }
    case CP_DOUBLE: {
        cp_double_info* pCpInfo = (cp_double_info*)pBaseCpInfo;

        fprintf(pStream, "%f", pCpInfo->value);
        break;
    }
    case CP_NAME_AND_TYPE: {
        cp_name_and_type_info* pCpInfo = (cp_name_and_type_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, ":");
        fprint_constant(pCpInfo->descriptorIndex, ppConstantPool, pStream);
        break;
    }
    case CP_UTF8: {
        cp_utf8_info* pCpInfo = (cp_utf8_info*)pBaseCpInfo;

        fprintf(pStream, "%.*s", pCpInfo->length, pCpInfo->bytes);
        break;
    }
    }
}

void fprint_constant_verbose(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pStream) {
    if (constantIndex == 0) {
        // TODO special case
        return;
    }

#define U16_MAX_NUMERALS_WIDTH 5 // 65535
#define DOUBLE_INDEX_WIDTH (1 + U16_MAX_NUMERALS_WIDTH + 1 + 1 + U16_MAX_NUMERALS_WIDTH) // #65525.#65535

    cp_info* pBaseCpInfo = get_constant(ppConstantPool, constantIndex);
    switch (*pBaseCpInfo) {
    case CP_CLASS: {
        cp_class_info* pCpInfo = (cp_class_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16, pCpInfo->nameIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(pCpInfo->nameIndex, ppConstantPool, pStream);
        break;
    }
    case CP_FIELDREF: {
        cp_fieldref_info* pCpInfo = (cp_fieldref_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16":%"PRIu16, pCpInfo->classIndex, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_METHODREF: {
        cp_methodref_info* pCpInfo = (cp_methodref_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16":%"PRIu16, pCpInfo->classIndex, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_IMETHODREF: {
        cp_imethodref_info* pCpInfo = (cp_imethodref_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16":%"PRIu16, pCpInfo->classIndex, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_STRING: {
        cp_string_info* pCpInfo = (cp_string_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16, pCpInfo->stringIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_INTEGER: {
        cp_integer_info* pCpInfo = (cp_integer_info*)pBaseCpInfo;
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_FLOAT: {
        cp_float_info* pCpInfo = (cp_float_info*)pBaseCpInfo;
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_LONG: {
        cp_long_info* pCpInfo = (cp_long_info*)pBaseCpInfo;
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_DOUBLE: {
        cp_double_info* pCpInfo = (cp_double_info*)pBaseCpInfo;
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_NAME_AND_TYPE: {
        cp_name_and_type_info* pCpInfo = (cp_name_and_type_info*)pBaseCpInfo;
        int printedCount = fprintf(pStream, "#%"PRIu16":%"PRIu16, pCpInfo->nameIndex, pCpInfo->descriptorIndex);
        fprintf(pStream, "%*s"PD"//"PD, DOUBLE_INDEX_WIDTH - printedCount, "");
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    case CP_UTF8: {
        cp_utf8_info* pCpInfo = (cp_utf8_info*)pBaseCpInfo;
        fprint_constant(constantIndex, ppConstantPool, pStream);
        break;
    }
    }
}

static uint8_t countNumerals(uint16_t value) {
    uint8_t counter = 1;
    while (value /= 10) {
        ++counter;
    }
    return counter;
}


void fprint_pool(uint16_t constantPoolCount, cp_info** ppConstantPool, FILE* pStream) {
    fprintf(pStream, "Constant pool:\n");

    uint8_t labelsMaxWidth = countNumerals(constantPoolCount);
    for (uint16_t i = 1; i < constantPoolCount; ++i) {
        fprintf(pStream, PD"%*s#", labelsMaxWidth - countNumerals(i), "");
        fprintf(pStream, "%"PRIu16 PD, i);

        cp_info* pBaseCpInfo = get_constant(ppConstantPool, i);
        switch (*pBaseCpInfo) {
        case CP_CLASS: {
            fprintf(pStream, "Class:             "PD);
            break;
        }
        case CP_FIELDREF: {
            fprintf(pStream, "Fieldref:          "PD);
            break;
        }
        case CP_METHODREF: {
            fprintf(pStream, "Methodref:         "PD);
            break;
        }
        case CP_IMETHODREF: {
            fprintf(pStream, "InterfaceMethodref:"PD);
            break;
        }
        case CP_STRING: {
            fprintf(pStream, "String:            "PD);
            break;
        }
        case CP_INTEGER: {
            fprintf(pStream, "Integer:           "PD);
            break;
        }
        case CP_FLOAT: {
            fprintf(pStream, "Float:             "PD);
            break;
        }
        case CP_LONG: {
            fprintf(pStream, "Long:              "PD);
            break;
        }
        case CP_DOUBLE: {
            fprintf(pStream, "Double:            "PD);
            break;
        }
        case CP_NAME_AND_TYPE: {
            fprintf(pStream, "NameAndType:       "PD);
            break;
        }
        case CP_UTF8: {
            fprintf(pStream, "Utf8:              "PD);
            break;
        }
        }

        fprint_constant_verbose(i, ppConstantPool, pStream);
        fprintf(pStream, "\n");
    } 
}
