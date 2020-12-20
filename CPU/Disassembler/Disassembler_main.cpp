#include "Disassembler.h"

//============================================================

int main(int argc, const char* argv[])
{
    FILE* bytecode = fopen(argv[1], "rb");

    if (bytecode == nullptr)
    {
        printf("File is not found, ERROR\n");
        return 1;
    }

    char* buffer = Bytecode_buffer(bytecode);

    fclose(bytecode);

    FILE* output = fopen("files/Output.txt", "wb");

    Disassembling(buffer, output);

    free(buffer);
    fclose(output);
}

