#include "../libr/Onegin.h"
#include "../libr/Stack.h"
#include "../libr/DSL.h"
//============================================================

const char Version = 3;
const unsigned char Signature = 0xDA;

//============================================================

enum Mods
{
    #define DEF_COMMAND(name, number, hash, argc, code)
    #define DEF_MOD(name, num) name = num,
    #define DEF_REG(name, number, hash)

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG
};

//============================================================

char* Bytecode_buffer(FILE* file);

void Disassembling(char* buffer, FILE* output);

int Unknown_command(char cmd);

const char* Name_code(char cmd);

int Argument_command(char cmd);
