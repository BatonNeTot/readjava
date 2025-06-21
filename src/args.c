#include "args.h"

#include "common.h"

#define HELP_MESSAGE \
"Usage: readjava <option(s)> java-class-file(s)\n"\
PD"Display information about the contents of Java virtual machine class format files\n"\
PD"Options are:\n"\
PD PD"-a --all    "PD"Equivalent to: -cpfm\n"\
PD PD"-c          "PD"Display class info\n"\
PD PD"-p          "PD"Display constant pool content\n"\
PD PD"-f          "PD"Display fields info\n"\
PD PD"-m          "PD"Display methods info\n"\
PD PD"-h --help   "PD"Display this information\n"\
PD PD"-v --version"PD"Display the version number of readjava\n"\


#define VERSION_MESSAGE \
"readjava " READJAVA_VERSION"\n"\
"Supports Java virtual machine class format files up to JDK 1.2"


static void fprint_and_exit(FILE* pStream, const char* message, int exitCode) {
    fprintf(pStream, message);
    exit(exitCode);
}

void parse_args(args_info* pArgs, int argc, char** argv, FILE* pStream) {
    *pArgs = (args_info){0};

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            ++pArgs->filenamesCount;
            continue;
        }

        if (argv[i][1] == '\0') {
            continue;
        }

        if (argv[i][1] == '-') {
            const char* flag = &argv[i][2];

#define if_arg_equal(literal)\
for (size_t length = strlen(flag); length == sizeof(literal) - 1 && memcmp(flag, literal, sizeof(literal) - 1) == 0; length = 0)

            if_arg_equal("all") {
                pArgs->classInfo = true;
                pArgs->constantPool = true;
                pArgs->fields = true;
                pArgs->methods = true;
                continue;
            }
            
            if_arg_equal("help") {
                fprint_and_exit(pStream, HELP_MESSAGE, 0);
                continue;
            }
            
            if_arg_equal("version") {
                fprint_and_exit(pStream, VERSION_MESSAGE, 0);
                continue;
            }

            fprintf(stderr, "readjava: unknown flag -- '%s'\n", flag);
            fprint_and_exit(stderr, HELP_MESSAGE, -1);
            continue;
        }

        for (int j = 1; argv[i][j] != '\0'; ++j) {
            switch(argv[i][j]) {
                case 'a': {
                    pArgs->classInfo = true;
                    pArgs->constantPool = true;
                    pArgs->fields = true;
                    pArgs->methods = true;
                    break;
                }
                case 'c': {
                    pArgs->classInfo = true;
                    break;
                }
                case 'p': {
                    pArgs->constantPool = true;
                    break;
                }
                case 'f': {
                    pArgs->fields = true;
                    break;
                }
                case 'm': {
                    pArgs->methods = true;
                    break;
                }
                case 'h': {
                    fprint_and_exit(pStream, HELP_MESSAGE, 0);
                }
                case 'v': {
                    fprint_and_exit(pStream, VERSION_MESSAGE, 0);
                }
                default:
                    fprintf(stderr, "readjava: unknown flag -- '%c'\n", argv[i][j]);
                    fprint_and_exit(stderr, HELP_MESSAGE, -1);
            }
        }
    }

    if (!(pArgs->classInfo || pArgs->constantPool || pArgs->fields || pArgs->methods) 
        || pArgs->filenamesCount == 0) {
        fprint_and_exit(pStream, HELP_MESSAGE, 0);
    }

    pArgs->ppFilenames = ALLOC(sizeof(const char*) * pArgs->filenamesCount);

    uint32_t filenamesCounter = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            continue;
        }
        
        pArgs->ppFilenames[filenamesCounter++] = argv[i];
    }
}

void fprint_info(args_info* pArgs, FILE* pStream) {
    for (uint32_t i = 0; i < pArgs->filenamesCount; ++i) {
        bool addNewLine = false;

        if (i > 0) {
            fprintf(pStream, "\n");
        }

        class_info classInfo;

        if (read_class(&classInfo, pArgs->ppFilenames[i]) && pArgs->filenamesCount > 1) {
            fprintf(pStream, "ClassFile:"PD"%s\n", pArgs->ppFilenames[i]);
        }

        if (pArgs->classInfo) {
            addNewLine = true;

            fprint_class(&classInfo, pStream);
        }

        if (pArgs->constantPool) {
            if (addNewLine) {
                fprintf(pStream, "\n");
            }
            addNewLine = true;

            fprint_pool(classInfo.constantPoolCount, classInfo.ppConstantPool, pStream);
        }
        
        if (pArgs->fields) {
            if (addNewLine) {
                fprintf(pStream, "\n");
            }
            addNewLine = true;

            if (classInfo.fieldsCount == 0) {
                fprintf(pStream, "There are no fields in this file.\n");
            } else

            for (uint16_t i = 0; i < classInfo.fieldsCount; ++i) {
                if (i != 0) {
                    fprintf(pStream, "\n");
                }

                field_info* pField = classInfo.pFields + i;

                fprint_field(pField, classInfo.ppConstantPool, pStream);
            }
        }
        
        if (pArgs->methods) {
            if (addNewLine) {
                fprintf(pStream, "\n");
            }
            addNewLine = true;

            if (classInfo.methodsCount == 0) {
                fprintf(pStream, "There are no methods in this file.\n");
            } else

            for (uint16_t i = 0; i < classInfo.methodsCount; ++i) {
                if (i != 0) {
                    fprintf(pStream, "\n");
                }

                method_info* pMethod = classInfo.pMethods + i;

                fprint_method(pMethod, classInfo.ppConstantPool, pStream);
            }
        }
    }
}