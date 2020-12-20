#include "Disassembler.h"

//========================================================================

char* Bytecode_buffer(FILE* file)
{
    assert(file != nullptr);

    size_t size = Size_file(file);

    char* buffer = (char*) calloc(size, sizeof(char));

    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, file);

    return buffer;
};

void Disassembling(char* buffer, FILE* output)
{
    assert(buffer != nullptr);
    assert(output != nullptr);

    fprintf(output, "Disassembler version: %d\n"
                    "Disassembler signature: %X\n\n\n"
                    "Code version: %d\n"
                    "Code signature: %X\n", 
                    Version, Signature, buffer[0], (unsigned char)buffer[1]);

    size_t count_cmd = *(size_t*)(&buffer[10]);
    size_t rip = 18;

    fprintf(output, "  Address  Number cmd     Code       Text\n");

    for (size_t num_current_cmd = 0; num_current_cmd < count_cmd; ++num_current_cmd)
    {   
        char cmd = buffer[rip];

        fprintf(output, "%#9x %11d %5d ", &buffer[rip] - buffer - 18, num_current_cmd + 1, cmd);

        if (Unknown_command(cmd))
        {
            fprintf(output, "Unknown command\n");
            printf("WARNING: UNKNOWN COMMAND - Number current command %d: %s\n", num_current_cmd, cmd);
            continue;
        }

        if (Argument_command(cmd))
        {
            switch (buffer[rip + 1])
            {
                case MOD_EMPTY:
                    fprintf(output, "%1d %11s\n", MOD_EMPTY, Name_code(cmd));
                    rip += sizeof(char);
                    break;

                case MOD_POINTER:
                    fprintf(output, "%1d %2ld %8s %2lu\n", MOD_POINTER, *(size_t*)(&buffer[rip + 2]), Name_code(cmd), *(size_t*)(&buffer[rip + 2]));
                    rip += sizeof(char) + sizeof(size_t);
                    break;

                case MOD_RA0: 
                    fprintf(output, "%1d %2lg %8s [%lg]\n", MOD_RA0, *(element_t*)(&buffer[rip + 2]), Name_code(cmd), *(element_t*)(&buffer[rip + 2]));
                    rip += sizeof(char) + sizeof(element_t);
                    break;

                case MOD_0A0: 
                    fprintf(output, "%1d %2lg %8s %lg\n", MOD_0A0, *(element_t*)(&buffer[rip + 2]), Name_code(cmd), *(element_t*)(&buffer[rip + 2]));
                    rip += sizeof(char) + sizeof(element_t);
                    break;

                case MOD_00R:
                    switch (buffer[rip + 2])
                    {
                        #define DEF_COMMAND(name, number, hash, argc, code)
                        #define DEF_MOD(name, number)
                        #define DEF_REG(name, num, hash)                                                  \
                            case num:                                                                     \
                                fprintf(output, "%1d %2d %8s %s\n", MOD_00R, num, Name_code(cmd), #name); \
                                rip += 2 * sizeof(char);                                                  \
                                break;

                        #include "../libr/command.h"

                        #undef DEF_COMMAND
                        #undef DEF_MOD
                        #undef DEF_REG

                        default:
                            fprintf(output, "unknown register\n");
                            printf("WARNING: UNKNOWN REGISTER - CMD: %s, REG: %d\n", Name_code(cmd), buffer[rip + 2]);

                            rip += 2 * sizeof(char);

                            break;
                    }
                    break;
                
                case MOD_R0R:
                    switch (buffer[rip + 2])
                    {
                        #define DEF_COMMAND(name, number, hash, argc, code)
                        #define DEF_MOD(name, number)
                        #define DEF_REG(name, num, hash)                                                  \
                            case num:                                                                     \
                                fprintf(output, "%1d %2d %8s [%s]\n", MOD_R0R, num, Name_code(cmd), #name); \
                                rip += 2 * sizeof(char);                                                  \
                                break;

                        #include "../libr/command.h"

                        #undef DEF_COMMAND
                        #undef DEF_MOD
                        #undef DEF_REG

                        default:
                            fprintf(output, "unknown register\n");
                            printf("WARNING: UNKNOWN REGISTER - CMD: %s, REG: %d\n", Name_code(cmd), buffer[rip + 2]);

                            rip += 2 * sizeof(char);

                            break;
                    }
                    break;
                

                case MOD_RAR:
                    switch (*(&buffer[rip + 2] + sizeof(element_t)))
                    {
                        #define DEF_COMMAND(name, number, hash, argc, code)
                        #define DEF_MOD(name, number)
                        #define DEF_REG(name, num, hash)                                                                   \
                            case num:                                                                                      \
                                fprintf(output, "%1d %2lg %2d %8s [%2lg + %-s]\n", MOD_R0R, *(element_t*)(&buffer[rip + 2]), num, Name_code(cmd), *(element_t*)(&buffer[rip + 2], #name));\
                                rip += 2 * sizeof(char);                                                                   \
                                break;

                        #include "../libr/command.h"

                        #undef DEF_COMMAND
                        #undef DEF_MOD
                        #undef DEF_REG

                        default:
                            fprintf(output, "unknown register\n");
                            printf("WARNING: UNKNOWN REGISTER - CMD: %s, REG: %d\n", Name_code(cmd), buffer[rip + 2]);

                            rip += 2 * sizeof(char);

                            break;
                    }
                    break;
                
                case MOD_0AR:
                    switch (*(&buffer[rip + 2] + sizeof(element_t)))
                    {
                        #define DEF_COMMAND(name, number, hash, argc, code)
                        #define DEF_MOD(name, number)
                        #define DEF_REG(name, num, hash)                                                                   \
                            case num:                                                                                      \
                                fprintf(output, "%1d %2lg %2d %8s %2lg + %-s \n", MOD_R0R, *(element_t*)(&buffer[rip + 2]), num, Name_code(cmd), *(element_t*)(&buffer[rip + 2], #name));\
                                rip += 2 * sizeof(char);                                                                   \
                                break;

                        #include "../libr/command.h"

                        #undef DEF_COMMAND
                        #undef DEF_MOD
                        #undef DEF_REG

                        default:
                            fprintf(output, "unknown register\n");
                            printf("WARNING: UNKNOWN REGISTER - CMD: %s, REG: %d\n", Name_code(cmd), buffer[rip + 2]);

                            rip += 2 * sizeof(char);

                            break;
                    }
                    break;
            
                default: 
                    fprintf(output, "unknown mode\n");
                    printf("WARNING: UNKNOWN MODE - CMD: %s, MODE: %d\n",  Name_code(cmd), buffer[rip + 1]);
                    break;
            }
        }

        else
        {
            fprintf(output, " %12s\n", Name_code(cmd));
        }

        ++rip;
    }
}

int Unknown_command(char cmd)
{
    #define DEF_COMMAND(Name, Num, Hash, Argc, code) case Num: return 0;
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    switch (cmd)
    {
    #include "../libr/command.h"

    default:
        return 1;
    }

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG
}

const char* Name_code(char cmd)
{
    #define DEF_COMMAND(name, num, hash, argc, code) case num: return #name;
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    switch (cmd)
    {
    #include "../libr/command.h"
    }

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG

    return "\0";
}

int Argument_command(char cmd)
{
    #define DEF_COMMAND(name, num, hash, argc, code) case num: return argc;
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    switch (cmd)
    {
    #include "../libr/command.h"
    }

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG

    return 0;
}