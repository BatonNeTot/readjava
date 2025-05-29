#include "args.h"

#include "common.h"

static void fprint_help_and_exit(FILE* pStream) {
    fprintf(pStream, "HELP\n");
    exit(-1);
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
            // TODO
            continue;
        }

        for (int j = 1; argv[i][j] != '\0'; ++j) {
            switch(argv[i][j]) {
                case 'c':
                case 'C': {
                    pArgs->classInfo = true;
                    break;
                }
                case 'p':
                case 'P': {
                    pArgs->constantPool = true;
                    break;
                }
                case 'f':
                case 'F': {
                    pArgs->fields = true;
                    break;
                }
                case 'm':
                case 'M': {
                    pArgs->methods = true;
                    break;
                }
                case 'H': {
                    fprint_help_and_exit(pStream);
                }
                default:
                    fprintf(stderr, "Unknown flag -- '%c'.\n", argv[i][j]);
                    fprint_help_and_exit(pStream);
            }
        }
    }

    if (!(pArgs->classInfo || pArgs->constantPool || pArgs->fields || pArgs->methods) 
        || pArgs->filenamesCount == 0) {
        fprint_help_and_exit(pStream);
    }

    pArgs->ppFilenames = malloc(sizeof(const char*) * pArgs->filenamesCount);

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
        if (i > 0) {
            fprintf(pStream, "\n");
        }

        class_info classInfo;

        if (read_class(&classInfo, pArgs->ppFilenames[i]) && pArgs->filenamesCount > 1) {
            fprintf(pStream, "File:"PD"%s\n", pArgs->ppFilenames[i]);
        }

        if (pArgs->classInfo) {
            fprint_class(&classInfo, pStream);
        }

        if (pArgs->constantPool) {
            if (pArgs->classInfo) {
                fprintf(pStream, "\n");
            }

            fprint_pool(classInfo.constantPoolCount, classInfo.ppConstantPool, pStream);
        }
        
        if (pArgs->fields) {
            if (pArgs->constantPool) {
                fprintf(pStream, "\n");
            }

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
            if (pArgs->fields) {
                fprintf(pStream, "\n");
            }

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