#ifndef attr_info_h
#define attr_info_h

#include "cp_info.h"

#include "common.h"

typedef uint16_t attr_tag;

enum __ATTR_TAG {
    ATTR_UNKNOWN                = 0x0001,
    ATTR_CONSTANT_VALUE         = 0x0002,
    ATTR_CODE                   = 0x0004,
    ATTR_EXCEPTIONS             = 0x0008,
    ATTR_INNER_CLASSES          = 0x0010,
    ATTR_SYNTHETIC              = 0x0020,
    ATTR_SOURCE_FILE            = 0x0040,
    ATTR_LINE_NUMBER_TABLE      = 0x0080,
    ATTR_LOCAL_VARIABLE_TABLE   = 0x0100,
    ATTR_DEPRECATED             = 0x0200,
};

#define __ATTR_HEADER \
attr_tag tag; /* cache field for known names */\
uint16_t nameIndex;\
uint32_t length

// Common struct for non-standard attributes
typedef struct attr_info {
    __ATTR_HEADER;
    uint8_t info[0]; // size of '__ATTR_HEADER.length'
} attr_info;

// The ConstantValue Attribute
// Used in the field_info
typedef struct attr_constant_value {
    __ATTR_HEADER;
    uint16_t constantValueIndex;
} attr_constant_value;

typedef struct attr_code_exception {
    uint16_t startPc;
    uint16_t endPc;
    uint16_t  handlerPc;
    uint16_t  catchType;
} attr_code_exception;

// The Code Attribute
// Used in the method_info
typedef struct attr_code {
    __ATTR_HEADER;
    uint16_t maxStack;
    uint16_t maxLocals;
    uint16_t exceptionTableLength;
    uint16_t attributesCount;
    uint32_t codeLength;
    attr_info** attributes;
    uint8_t* pCode;
    attr_code_exception* pExceptionTable;
} attr_code;

// The Exceptions Attribute
// Used in the method_info
typedef struct attr_exceptions {
    __ATTR_HEADER;
    uint16_t numberOfExceptions;
    uint16_t exceptionIndexTable[0]; // size of 'numberOfExceptions'
} attr_exceptions;

typedef struct attr_inner_classes_info {
    uint16_t innerClassInfoIndex;
    uint16_t outerClassInfoIndex;
    uint16_t innerNameIndex;
    uint16_t innerClassAccessFlags;
} attr_inner_classes_info;

// The InnerClasses Attribute
// Used in the class_info
typedef struct attr_inner_classes {
    __ATTR_HEADER;
    uint16_t numberOfClasses;
    attr_inner_classes_info classes[0]; // size of 'numberOfClasses'
} attr_inner_classes;

// The Synthetic Attribute
// Used in the class_info, field_info, method_info
typedef struct attr_synthetic {
    __ATTR_HEADER;
} attr_synthetic;

// The SourceFile Attribute
// Used in the class_info
typedef struct attr_source_file {
    __ATTR_HEADER;
    uint16_t sourceFileIndex;
} attr_source_file;

typedef struct attr_line_number_table_entry {
    uint16_t startPc;
    uint16_t lineNumber;
} attr_line_number_table_entry;

// The LineNumberTable Attribute
// Used in the attr_code
typedef struct attr_line_number_table {
    __ATTR_HEADER;
    uint16_t lineNumberTableLength;
    attr_line_number_table_entry lineNumberTable[0]; // size of 'lineNumberTableLength'
} attr_line_number_table;

typedef struct attr_local_variable_table_entry {
    uint16_t startPc;
    uint16_t length;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t index;
} attr_local_variable_table_entry;

// The LocalVariableTable Attribute
// Used in the attr_code
typedef struct attr_local_variable_table {
    __ATTR_HEADER;
    uint16_t localVariableTableLength;
    attr_local_variable_table_entry localVariableTable[0]; // size of 'localVariableTableLength'
} attr_local_variable_table;

// The Deprecated Attribute
// Used in the class_info, field_info, method_info
typedef struct attr_deprecated {
    __ATTR_HEADER;
} attr_deprecated;

#undef __ATTR_HEADER

attr_info* read_attr(cp_info** ppConstantPool, FILE* pClassFile, const char* pFilename);

void free_attr(attr_info* pAttrInfo);

void fprint_attr(attr_info* pAttrInfo, cp_info** ppConstantPool, FILE* pStream);

#endif