#include "cp_info.h"

#include "common.h"

cp_tag* read_constant(FILE* pClassFile) {
    cp_tag tag;
    read_u8(tag);
    
    switch(tag) {
        case CP_CLASS: {
            cp_class_info* pInfo = malloc(sizeof(cp_class_info));
            pInfo->tag = tag;
            read_u16(pInfo->nameIndex);
            return (cp_tag*)pInfo;
        }
        case CP_FIELDREF: {
            cp_fieldref_info* pInfo = malloc(sizeof(cp_fieldref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CP_METHODREF: {
            cp_methodref_info* pInfo = malloc(sizeof(cp_methodref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CP_IMETHODREF: {
            cp_imethodref_info* pInfo = malloc(sizeof(cp_imethodref_info));
            pInfo->tag = tag;
            read_u16(pInfo->classIndex);
            read_u16(pInfo->nameAndTypeIndex);
            return (cp_tag*)pInfo;
        }
        case CP_STRING: {
            cp_string_info* pInfo = malloc(sizeof(cp_string_info));
            pInfo->tag = tag;
            read_u16(pInfo->stringIndex);
            return (cp_tag*)pInfo;
        }
        case CP_INTEGER: {
            cp_integer_info* pInfo = malloc(sizeof(cp_integer_info));
            pInfo->tag = tag;
            read_u32(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CP_FLOAT: {
            cp_float_info* pInfo = malloc(sizeof(cp_float_info));
            pInfo->tag = tag;
            read_u32(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CP_LONG: {
            cp_long_info* pInfo = malloc(sizeof(cp_long_info));
            pInfo->tag = tag;
            read_u64(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CP_DOUBLE: {
            cp_double_info* pInfo = malloc(sizeof(cp_double_info));
            pInfo->tag = tag;
            read_u64(pInfo->bytes);
            return (cp_tag*)pInfo;
        }
        case CP_NAME_AND_TYPE: {
            cp_name_and_type_info* pInfo = malloc(sizeof(cp_name_and_type_info));
            pInfo->tag = tag;
            read_u16(pInfo->nameIndex);
            read_u16(pInfo->descriptorIndex);
            return (cp_tag*)pInfo;
        }
        case CP_UTF8: {
            uint16_t length;
            read_u16(length);
            cp_utf8_info* pInfo = malloc(sizeof(cp_utf8_info) + sizeof(uint8_t) * length);
            pInfo->tag = tag;
            pInfo->length = length;
            read_block(pInfo->bytes, length);
            return (cp_tag*)pInfo;
        }
        default: 
            fprintf(stderr, "Unknown constant tag %02X.\n", tag);
            exit(-1);
    }
}

void fprint_constant(uint16_t constantIndex, cp_info** ppConstantPool, FILE* pFile) {
    if (constantIndex == 0) {
        // TODO special case
        return;
    }

    cp_info* pBaseCpInfo = get_constant(ppConstantPool, constantIndex);
    switch (*pBaseCpInfo) {
    case CP_CLASS: {
        cp_class_info* pCpInfo = (cp_class_info*)pBaseCpInfo;

        fprint_constant(pCpInfo->nameIndex, ppConstantPool, pFile);
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

        fprintf(pFile, "\"");
        fprint_constant(pCpInfo->stringIndex, ppConstantPool, pFile);
        fprintf(pFile, "\"");
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

        fprintf(pFile, "%.*s", pCpInfo->length, pCpInfo->bytes);
        break;
    }
    }
}
