#include "CPU.h"

//==============================================================

int CPU_construct(struct CPU* proc, FILE* file)
{
    assert(proc != nullptr);
    assert(file != nullptr);

    STACK_CONSTRUCT(&proc->stk,      10);
    STACK_CONSTRUCT(&proc->call_stk, 10);

    proc->RAM = (element_t*) calloc(powl(2, 24), sizeof(element_t));

    unsigned char buffer[18] = {};

    fread(buffer, sizeof(unsigned char), 18, file);

    if (Verification_code(buffer)) return 1;

    proc->size  = *(size_t*)(&buffer[2]);
    proc->n_cmd = *((size_t*)(&buffer[10]));

    proc->bytecode = (unsigned char*) calloc(proc->size, sizeof(unsigned char));
    proc->rip = 0;

    fread(proc->bytecode, sizeof(unsigned char), proc->size, file);

    for (int i = 0; i < 4; ++i)
    {
        (proc->registers)[i] = NAN;
    }

    return 0;
}

int Processing(struct CPU* proc)
{
    assert(proc != nullptr);

    for (size_t i = 0;; ++i)
    {
        if (Operation(proc, (proc->bytecode)[proc->rip])) return 1;

        // Stack_dump(stdout, &proc->stk);
        // Stack_dump(stdout, &proc->call_stk);
        // printf("Registers: %lg %lg %lg %lg \n", (proc->registers)[0], (proc->registers)[1], (proc->registers)[2], (proc->registers)[3]);

    }

    return 0;
}

int Operation(struct CPU* proc, int cmd)
{
    assert(proc != nullptr);
    assert(cmd != -1);

    const size_t size_cmd = sizeof(char);
    const size_t size_mod = sizeof(char);
    const size_t size_reg = sizeof(char);
    const size_t size_arg = sizeof(element_t);
    const size_t size_lbl = sizeof(size_t);

    switch (cmd)
    {
    #define DEF_COMMAND(name, number, hash, argc, code) case CMD_##name: code; break;
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG

    default:
        printf("Unknown command, ERROR\n"
               "Code: %X \n", cmd);
        return 1;
    }

    return 0;
}

void CPU_destruct(struct CPU* proc)
{
    assert(proc != nullptr);

    free(proc->bytecode);
    free(proc->RAM);
    
    Stack_destruct(&proc->stk);
    Stack_destruct(&proc->call_stk);

    proc->bytecode = nullptr;
    proc->RAM      = nullptr;
    
    proc->n_cmd = -1;
    proc->rip   = -1;
    proc->size  = -1;

    RRAX = NAN;
    RRBX = NAN;
    RRCX = NAN;
    RRDX = NAN;
}

int Verification_code(unsigned char* buffer)
{
    assert(buffer != nullptr);

    if (Version != buffer[0])
    {
        printf("WARNING: assembler version doesn't match CPU verision\n"
               "Assembler verison: %d\n"
               "CPU verison: %d\n", buffer[0], Version);
        return 0;
    }

    if (Signature != buffer[1])
    {
        printf("ERROR: assembler signature  doesn't match CPU signature\n"
               "Assembler signature: %X\n"
               "CPU signature: %X\n", buffer[1], Signature);
        return 1;
    }
    
    return 0;
}

