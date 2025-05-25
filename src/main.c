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
        if (argv[i][1] == '\0') {
            continue;
        }
        
        switch(argv[i][1]) {
            case 'c':
            case 'C': {
                printf("Class info:\n");
                printf("\tVersion %03"PRIu16".%03"PRIu16"\n", classInfo.majorVersion, classInfo.minorVersion);
                printf("\tClass \"0x%04"PRIx16"\"\n", classInfo.thisClass);
                printf("\tSuper \"0x%04"PRIx16"\"\n", classInfo.superClass);

                printf("\tAccess flags: ");
                print_access(classInfo.accessFlags, "\t", PUBLIC, "");
                print_access(classInfo.accessFlags, "\t", FINAL, "");
                print_access(classInfo.accessFlags, "\t", SUPER, "");
                print_access(classInfo.accessFlags, "\t", INTERFACE, "");
                print_access(classInfo.accessFlags, "\t", ABSTRACT, "");
                printf("\n");

                printf("\tInterfaces: %"PRIu16"\n", classInfo.interfacesCount);
                printf("\tFields: %"PRIu16"\n", classInfo.fieldsCount);
                printf("\tMethods: %"PRIu16"\n", classInfo.methodsCount);
                printf("\tAttributes: %"PRIu16"\n", classInfo.attributesCount);
                break;
            }
            default:
            printf("Unknown flag -%c, ignored.\n", argv[i][1]);
        }
    }

    return 0;
}