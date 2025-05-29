#include "args.h"

int main(int argc, char** argv) {
    args_info args;
    parse_args(&args, argc, argv, stdout);
    fprint_info(&args, stdout);

    return 0;
}