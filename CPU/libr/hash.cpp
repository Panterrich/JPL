#include "string.h"
#include "stdio.h"

unsigned int Hash(const char* cmd)
{
    unsigned int hash = 0;
    
    for (int i = 0; i < strlen(cmd); ++i)
    {
        hash += (unsigned char)(*(cmd + i));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

int main()
{
    #define DEF_COMMAND(name, num, hash, argc, code) printf("%s: 0x%x \n", #name, Hash(#name));
    #define DEF_REG(name, num, hash) printf("%s: 0x%x \n", #name, Hash(#name));
    #define DEF_MOD(name, num)
    #include "command.h"
    #undef DEF_COMMAND
    #undef DEF_REG
    #undef DEF_MOD
}