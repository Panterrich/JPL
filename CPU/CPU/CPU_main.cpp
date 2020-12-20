#include "CPU.h"

//======================================================

int main(int argc, char* argv[])
{
    FILE* code = fopen(argv[1], "rb");

    if (code == nullptr)
    {
        printf("File is not found, ERROR");
        return 1;
    }

    struct CPU proc = {};

    if (CPU_construct(&proc, code)) return 1;

    fclose(code);

    if (Processing(&proc)) return 1;

    CPU_destruct(&proc);

    return 0;
}

