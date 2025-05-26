#include "common.h"

#include "class_info.h"

int main(int argc, char** argv) {
    const char* targetFilename = NULL;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            continue;
        } 

        targetFilename = argv[i];
    }

    class_info classInfo;
    read_class(&classInfo, targetFilename);

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            continue;
        }
        
        for (int j = 1; argv[i][j] != '\0'; ++j) {
#define PD PADDING
            if (i != 1 || j != 1) {
                fprintf(stdout, "\n");
            }

            switch(argv[i][j]) {
                case 'c':
                case 'C': {
                    fprintf(stdout, "Class info:\n");
                    fprintf(stdout, PD"Version     "PD"%04"PRIX16".%04"PRIX16"\n", classInfo.majorVersion, classInfo.minorVersion);
                    
                    fprintf(stdout, PD"Class       "PD);
                    fprint_constant(classInfo.thisClassIndex, classInfo.ppConstantPool, stdout);
                    fprintf(stdout, "\n");

                    fprintf(stdout, PD"Super       "PD);
                    fprint_constant(classInfo.superClassIndex, classInfo.ppConstantPool, stdout);
                    fprintf(stdout, "\n");

                    fprintf(stdout, PD"Access flags"PD);
                    fprint_access(stdout, classInfo.accessFlags, "", PUBLIC, PD);
                    fprint_access(stdout, classInfo.accessFlags, "", FINAL, PD);
                    fprint_access(stdout, classInfo.accessFlags, "", SUPER, PD);
                    fprint_access(stdout, classInfo.accessFlags, "", INTERFACE, PD);
                    fprint_access(stdout, classInfo.accessFlags, "", ABSTRACT, PD);
                    fprintf(stdout, "\n");

                    fprintf(stdout, PD"Interfaces  "PD);
                    for (uint16_t i = 0; i < classInfo.interfacesCount; ++i) {
                        fprint_constant(classInfo.pInterfaces[i], classInfo.ppConstantPool, stdout);
                        fprintf(stdout, PD);
                    }
                    fprintf(stdout, "\n");

                    fprintf(stdout, PD"Fields      "PD"%"PRIu16"\n", classInfo.fieldsCount);
                    fprintf(stdout, PD"Methods     "PD"%"PRIu16"\n", classInfo.methodsCount);

                    if (classInfo.attributesCount > 0) {
                        fprintf(stdout, PD"Attributes\n");

                        for (uint16_t i = 0; i < classInfo.attributesCount; ++i) {
                            fprintf(stdout, PD PD);
                            fprint_attr(classInfo.attributes[i], classInfo.ppConstantPool, stdout);
                            fprintf(stdout, "\n");
                        }
                    }
                    break;
                }
                case 'f':
                case 'F': {
                    if (classInfo.fieldsCount == 0) {
                        fprintf(stdout, "There are no fields in this file.\n");
                    }

                    for (uint16_t i = 0; i < classInfo.fieldsCount; ++i) {
                        if (i != 0) {
                            fprintf(stdout, "\n");
                        }

                        field_info* pField = classInfo.pFields + i;

                        fprintf(stdout, "Field info:\n");

                        fprintf(stdout, PD"Name      "PD);
                        fprint_constant(pField->nameIndex, classInfo.ppConstantPool, stdout);
                        fprintf(stdout, "\n");
                        
                        fprintf(stdout, PD"Descriptor"PD);
                        fprint_constant(pField->descriptorIndex, classInfo.ppConstantPool, stdout);
                        fprintf(stdout, "\n");

                        if (pField->attributesCount > 0) {
                            fprintf(stdout, PD"Attributes:\n");
                            for (uint16_t i = 0; i < pField->attributesCount; ++i) {
                                fprintf(stdout, PD PD);
                                fprint_attr(pField->attributes[i], classInfo.ppConstantPool, stdout);
                                fprintf(stdout, "\n");
                            }
                        }
                    }
                    break;
                }
                case 'm':
                case 'M': {
                    if (classInfo.fieldsCount == 0) {
                        fprintf(stdout, "There are no methods in this file.\n");
                    }

                    for (uint16_t i = 0; i < classInfo.methodsCount; ++i) {
                        if (i != 0) {
                            fprintf(stdout, "\n");
                        }

                        method_info* pMethod = classInfo.pMethods + i;

                        fprintf(stdout, "Method info:\n");

                        fprintf(stdout, PD"Name      "PD);
                        fprint_constant(pMethod->nameIndex, classInfo.ppConstantPool, stdout);
                        fprintf(stdout, "\n");
                        
                        fprintf(stdout, PD"Descriptor"PD);
                        fprint_constant(pMethod->descriptorIndex, classInfo.ppConstantPool, stdout);
                        fprintf(stdout, "\n");

                        if (pMethod->attributesCount > 0) {
                            fprintf(stdout, PD"Attributes:\n");
                            for (uint16_t i = 0; i < pMethod->attributesCount; ++i) {
                                fprintf(stdout, PD PD);
                                fprint_attr(pMethod->attributes[i], classInfo.ppConstantPool, stdout);
                                fprintf(stdout, "\n");
                            }
                        }
                    }
                    break;
                }
                default:
                    fprintf(stderr, "Unknown flag -%c.\n", argv[i][j]);
                    exit(-1);
            }
        }
    }

    return 0;
}