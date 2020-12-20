#include "../libr/Onegin.h"
#include "../libr/Stack.h"
#include "../libr/DSL.h"
#include <assert.h>
#include <string.h>
#include <math.h>

//============================================================

const char Version = 3;
const unsigned char Signature = 0xDA;  

//============================================================

struct Label
{
    unsigned int hash;
    size_t address;
};

//============================================================

int Compile_pass(struct Text* input_text, struct Label* labels);

int Compilation(struct Text* input_text, FILE* listing, FILE* code, struct Label* labels);

int Next_word(char* begin, char* word, size_t* lshift, size_t* rshift);

int Code_cmd(const char* cmd);

int Arg_command(int cmd);

unsigned int Hash(const char* cmd);

void Str_uppercase(char* string);

int Code_reg(const char* reg);

void Free_mem(char* command, char* reg, char* label, char* current_word);

//============================================================

enum Commands 
{
    #define DEF_COMMAND(name, number, hash, argc, code) CMD_##name = number,
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 
};

enum Mods
{
    #define DEF_COMMAND(name, number, hash, argc, code)
    #define DEF_MOD(name, number) name = number,
    #define DEF_REG(name, number, hash) 

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 
};

enum Registers
{
    #define DEF_COMMAND(name, number, hash, argc, code)
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash) REG_##name = number,

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 
};

//=============================================================