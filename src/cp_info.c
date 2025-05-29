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

void fprint_constant_short(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pStream) {
    if (constantIndex == 0) {
        // TODO special case
        return;
    }

    cp_info* pBaseCpInfo = get_constant(ppConstantPool, constantIndex);
    switch (*pBaseCpInfo) {
    case CP_CLASS: {
        cp_class_info* pCpInfo = (cp_class_info*)pBaseCpInfo;

        fprint_constant_short(pCpInfo->nameIndex, ppConstantPool, pStream);
        break;
    }
    case CP_FIELDREF: {
        break;
    }
    case CP_METHODREF: {
        break;
    }
    case CP_IMETHODREF: {
        break;
    }
    case CP_STRING: {
        cp_string_info* pCpInfo = (cp_string_info*)pBaseCpInfo;

        fprintf(pStream, "\"");
        fprint_constant_short(pCpInfo->stringIndex, ppConstantPool, pStream);
        fprintf(pStream, "\"");
        break;
    }
    case CP_INTEGER: {
        break;
    }
    case CP_FLOAT: {
        break;
    }
    case CP_LONG: {
        break;
    }
    case CP_DOUBLE: {
        break;
    }
    case CP_NAME_AND_TYPE: {
        break;
    }
    case CP_UTF8: {
        cp_utf8_info* pCpInfo = (cp_utf8_info*)pBaseCpInfo;

        fprintf(pStream, "%.*s", pCpInfo->length, pCpInfo->bytes);
        break;
    }
    }
}

static void fprint_constant(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pStream) {
    if (constantIndex == 0) {
        // TODO special case
        return;
    }

    cp_info* pBaseCpInfo = get_constant(ppConstantPool, constantIndex);
    switch (*pBaseCpInfo) {
    case CP_CLASS: {
        cp_class_info* pCpInfo = (cp_class_info*)pBaseCpInfo;

        fprintf(pStream, PD"Class:\n");
        fprintf(pStream, PD PD"Name      "PD);
        fprint_constant_short(pCpInfo->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_FIELDREF: {
        cp_fieldref_info* pCpInfo = (cp_fieldref_info*)pBaseCpInfo;

        fprintf(pStream, PD"Fieldref:\n");
        fprintf(pStream, PD PD"Class     "PD);
        fprint_constant_short(pCpInfo->classIndex, ppConstantPool, pStream);
        cp_name_and_type_info* pCpNameAndType = (cp_name_and_type_info*)get_constant(ppConstantPool, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "\n"PD PD"Name      "PD);
        fprint_constant_short(pCpNameAndType->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n"PD PD"Descriptor"PD);
        fprint_constant_short(pCpNameAndType->descriptorIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_METHODREF: {
        cp_methodref_info* pCpInfo = (cp_methodref_info*)pBaseCpInfo;

        fprintf(pStream, PD"Methodref:\n");
        fprintf(pStream, PD PD"Class     "PD);
        fprint_constant_short(pCpInfo->classIndex, ppConstantPool, pStream);
        cp_name_and_type_info* pCpNameAndType = (cp_name_and_type_info*)get_constant(ppConstantPool, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "\n"PD PD"Name      "PD);
        fprint_constant_short(pCpNameAndType->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n"PD PD"Descriptor"PD);
        fprint_constant_short(pCpNameAndType->descriptorIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_IMETHODREF: {
        cp_imethodref_info* pCpInfo = (cp_imethodref_info*)pBaseCpInfo;

        fprintf(pStream, PD"InterfaceMethodref:\n");
        fprintf(pStream, PD PD"Class     "PD);
        fprint_constant_short(pCpInfo->classIndex, ppConstantPool, pStream);
        cp_name_and_type_info* pCpNameAndType = (cp_name_and_type_info*)get_constant(ppConstantPool, pCpInfo->nameAndTypeIndex);
        fprintf(pStream, "\n"PD PD"Name      "PD);
        fprint_constant_short(pCpNameAndType->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n"PD PD"Descriptor"PD);
        fprint_constant_short(pCpNameAndType->descriptorIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_STRING: {
        cp_string_info* pCpInfo = (cp_string_info*)pBaseCpInfo;

        fprintf(pStream, PD"String:\n"PD PD);
        fprint_constant_short(pCpInfo->stringIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_INTEGER: {
        cp_integer_info* pCpInfo = (cp_integer_info*)pBaseCpInfo;

        fprintf(pStream, PD"Integer:\n"PD PD"%"PRId32"\n", pCpInfo->value);
        break;
    }
    case CP_FLOAT: {
        cp_float_info* pCpInfo = (cp_float_info*)pBaseCpInfo;

        fprintf(pStream, PD"Float:\n"PD PD"%f\n", pCpInfo->value);
        break;
    }
    case CP_LONG: {
        cp_long_info* pCpInfo = (cp_long_info*)pBaseCpInfo;

        fprintf(pStream, PD"Long:\n"PD PD"%"PRId64"\n", pCpInfo->value);
        break;
    }
    case CP_DOUBLE: {
        cp_double_info* pCpInfo = (cp_double_info*)pBaseCpInfo;

        fprintf(pStream, PD"Double:\n"PD PD"%f\n", pCpInfo->value);
        break;
    }
    case CP_NAME_AND_TYPE: {
        cp_name_and_type_info* pCpInfo = (cp_name_and_type_info*)pBaseCpInfo;

        fprintf(pStream, PD"NameAndType:\n");
        fprintf(pStream, PD PD"Name      "PD);
        fprint_constant_short(pCpInfo->nameIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n"PD PD"Descriptor"PD);
        fprint_constant_short(pCpInfo->descriptorIndex, ppConstantPool, pStream);
        fprintf(pStream, "\n");
        break;
    }
    case CP_UTF8: {
        cp_utf8_info* pCpInfo = (cp_utf8_info*)pBaseCpInfo;

        fprintf(pStream, PD"Utf8:\n"PD PD);
        fprintf(pStream, "%.*s", pCpInfo->length, pCpInfo->bytes);
        fprintf(pStream, "\n");
        break;
    }
    }
}


void fprint_pool(uint16_t constantPoolCount, cp_info** ppConstantPool, FILE* pStream) {
    fprintf(pStream, "Constant pool:\n");

    for (uint16_t i = 1; i < constantPoolCount - 1; ++i) {
        fprint_constant(i, ppConstantPool, pStream);
    } 
}
