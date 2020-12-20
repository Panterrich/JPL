#include "../libr/Guard.h"
#include "../libr/DSL.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

//==============================================================

const char Version = 3;
const unsigned char Signature = 0xDA;  

//==============================================================
struct CPU
{
    struct Stack stk;
    struct Stack call_stk;
    element_t* RAM;

    unsigned char* bytecode;
    size_t rip;

    size_t size;
    size_t n_cmd;

    element_t registers[5];
};

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

//=====================================================

int CPU_construct(struct CPU* proc, FILE* file);

void CPU_destruct(struct CPU* proc);

int Processing(struct CPU* proc);

int Operation(struct CPU* proc, int cmd);

int Verification_code(unsigned char* buffer);
