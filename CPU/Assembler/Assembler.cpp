#include "Assembler.h"

//========================================================================

int Compile_pass(struct Text* input_text, struct Label* labels)
{
    assert(input_text != nullptr);
    assert(labels     != nullptr);

    FILE* listing = fopen("files/ASM_listing.txt", "wb");
    FILE* code    = fopen("files/code.txt",        "wb");

    if (Compilation(input_text, listing, code, labels)) return 1;

    fclose(listing);
    fclose(code);

    return 0;
}

int Compilation(struct Text* input_text, FILE* listing, FILE* code, struct Label* labels)
{
    assert(input_text != nullptr);
    assert(listing    != nullptr);
    assert(code       != nullptr);
    
    static size_t count_label = 0;

    size_t count_cmd   = 0;
    size_t size        = 0;

    char* buffer  = (char*) calloc(input_text->n_lines, sizeof(element_t) + 2 * sizeof(char));
    char* pointer = buffer;

    fprintf(listing, 
        "Assembler version: %d\n"
        "Assembler signature: %X\n\n\n"
        "  Adress   Number     Code     Text\n", Version, Signature);

    for (int index_strings = 0; index_strings < input_text->n_lines; ++index_strings)
    {   
        char* label        = (char*) calloc((input_text->lines)[index_strings].len, sizeof(char));
        char* command      = (char*) calloc((input_text->lines)[index_strings].len, sizeof(char));
        char* reg          = (char*) calloc((input_text->lines)[index_strings].len, sizeof(char));
        char* current_word = (char*) calloc((input_text->lines)[index_strings].len, sizeof(char));

        char*  current_pointer = (input_text->lines)[index_strings].str;
        size_t current_lshift = 0;
        size_t current_rshift = 0;

        double value = NAN;

        if (Next_word(current_pointer, label, &current_lshift, &current_rshift))
        {
            if (label[current_rshift - current_lshift - 1] == ':')
            {
                label[current_rshift - current_lshift - 1] = '\0';

                Str_uppercase(label);
                unsigned int hlabel = Hash(label);

                size_t count_no_matches = 0;
                
                for (int i = 0; i < count_label; ++i)
                {
                    if (hlabel != labels[i].hash)
                    {
                        ++count_no_matches;
                    }
                    
                    else 
                    {
                        break;
                    }
                }

                if (count_no_matches == count_label)
                {
                    labels[count_label].hash    = hlabel; 
                    labels[count_label].address = pointer - buffer;
                    ++count_label;
                }
                
                current_pointer += current_rshift;
            }
        }

        if (Next_word(current_pointer, command, &current_lshift, &current_rshift))
        {
            current_pointer += current_rshift;

            Str_uppercase(command);
            char cmd = Code_cmd(command);
        
            if (cmd != -1)
            {   
                ++count_cmd;
                *pointer = cmd;
                ++pointer;

                if (Arg_command(cmd))
                {
                    Next_word(current_pointer, current_word, &current_lshift, &current_rshift);
                    
                    if (current_word[0] == ':')
                    {
                        if ((cmd >= 30) && (cmd < 38))
                        {
                            Str_uppercase(current_word);
                            unsigned int label_hash = Hash(current_word + 1);
                            unsigned int plug = 0;

                            for (int i = 0; i < count_label; ++i)
                            {
                                if (label_hash == labels[i].hash)
                                {
                                    *pointer = MOD_POINTER;
                                    ++pointer;

                                    *((size_t*)pointer) = labels[i].address;
                                    pointer += sizeof(size_t);

                                    plug = 1;

                                    fprintf(listing, "%#8x %8lu %3lu %lu %2lu %6s :%s \n", 
                                            pointer - buffer - 2 * sizeof(char) - sizeof(size_t),
                                            count_cmd, cmd, MOD_POINTER, labels[i].address, command, current_word + 1);
                                }
                            }

                            if (plug == 0)
                            {
                                *pointer = MOD_POINTER;
                                ++pointer;

                                *((size_t*)pointer) = -1;
                                pointer += sizeof(size_t);
                            }
                        }

                        else 
                        {
                            fprintf(listing, "%#8x %8lu %5lu This command don't use label: %8s %s \n", 
                                    pointer - buffer - sizeof(char),
                                    count_cmd, cmd, command, reg + 1);
                                            
                            printf("ERROR: This command don't use label: %8s %s \n", command, reg + 1);

                            return 1;
                        }

                        Free_mem(command, reg, label, current_word);
                        continue;
                    }

                    else if (current_word[0] == '[')
                    {
                        int argc = sscanf(current_pointer, " %s %s %s", reg, current_word, label);

                        switch (argc)
                        {
                        case 1:
                            {
                            sscanf(current_pointer, " %s", reg);
                            
                            if (reg[strlen(reg) - 1] != ']')
                            {
                                fprintf(listing, "%#8x %8lu %6lu Syntax error %8s %s\n",
                                    pointer - buffer - sizeof(char),  
                                    count_cmd, cmd , command, reg + 1);
                                printf("ERROR: Syntax error: %8s %s \n", command, reg + 1);

                                return 1;
                            }

                            else 
                            {
                                reg[strlen(reg) - 1] = '\0';
                                Str_uppercase(reg);

                                char number_register = Code_reg(reg + 1);

                                if (number_register != -1)
                                {
                                    *pointer = MOD_R0R;
                                    ++pointer;

                                    *pointer = number_register;
                                    ++pointer;

                                    fprintf(listing, "%#8x %8lu %3lu %1lu %2lu %8s %s\n", 
                                            pointer - buffer - 3 * sizeof(char),
                                            count_cmd, cmd, MOD_R0R, number_register, command, reg + 1);
                                    
                                }

                                else if (sscanf(reg + 1, "%lg", &value ) == 1)
                                {
                                    *pointer = MOD_RA0;
                                    ++pointer;
                                    
                                    *((element_t*)pointer) = value;
                                    pointer += sizeof(element_t);

                                    fprintf(listing, "%#8x %8lu %3lu %1lu %2lg %8s %lg\n", 
                                            pointer - buffer - sizeof(element_t) - 2 * sizeof(char),
                                            count_cmd, cmd, MOD_RA0, value,  command, value);
                                }
                                
                                else 
                                {
                                    fprintf(listing, "%#8x %8lu %6lu Syntax error %8s %s\n",
                                            pointer - buffer - sizeof(char),  
                                            count_cmd, cmd , command, reg + 1);
                                    printf("ERROR: Syntax error: %8s %s \n", command, reg + 1);

                                    return 1;
                                }

                            }

                            break;
                            }

                        case 3:
                        {
                            if (current_word[0] != '+')
                            {
                                fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                    pointer - buffer - sizeof(char),  
                                    count_cmd, cmd , command, reg, current_word, label);
                                printf("ERROR: Syntax error: %s %s %s %s \n", command, reg, current_word, label);

                                return 1;
                            }

                            if (label[strlen(label) - 1] != ']')
                            {
                                fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                    pointer - buffer - sizeof(char),  
                                    count_cmd, cmd , command, reg + 1, current_word, label);
                                printf("ERROR: Syntax error: %s %s %s %s \n", command, reg + 1, current_word, label);

                                return 1;
                            }

                            else
                            {
                                label[strlen(label) - 1] = '\0';
                                Str_uppercase(reg);
                                Str_uppercase(label);

                                char number_register = Code_reg(reg + 1);

                                if (number_register != -1)
                                {
                                    if (sscanf(label, "%lg", &value ) == 1)
                                    {
                                        *pointer = MOD_RAR;
                                        ++pointer;

                                        *((element_t*)pointer) = value;
                                        pointer += sizeof(element_t);

                                        *pointer = number_register;
                                        ++pointer;

                                        fprintf(listing, "%#8x %8lu %2lu %1lu %2lg %1lu %8s %3lg %1s \n", 
                                            pointer - buffer - sizeof(element_t) - 3 * sizeof(char),
                                            count_cmd, cmd, MOD_RAR, value, number_register,  command, value, reg + 1);
                                    }
                                    else
                                    {
                                        fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                                pointer - buffer - sizeof(char),  
                                                count_cmd, cmd , command, reg + 1, current_word, label);
                                        printf("ERROR: Syntax error: %s %s %s %s \n", command, reg + 1, current_word, label);

                                        return 1;
                                    }
                                }

                                else
                                {
                                    number_register = Code_reg(label);

                                    if (number_register != -1)
                                    {
                                        if (sscanf(reg + 1, "%lg", &value ) == 1)
                                        {
                                            *pointer = MOD_RAR;
                                            ++pointer;

                                            *((element_t*)pointer) = value;
                                            pointer += sizeof(element_t);

                                            *pointer = number_register;
                                            ++pointer;

                                            fprintf(listing, "%#8x %8lu %2lu %1lu %2lg %1lu %8s %3lg %1lu \n", 
                                                pointer - buffer - sizeof(element_t) - 3 * sizeof(char),
                                                count_cmd, cmd, MOD_RAR, value, number_register,  command, value, label);
                                        }

                                        else
                                        {
                                            fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                                    pointer - buffer - sizeof(char),  
                                                    count_cmd, cmd , command, reg + 1, current_word, label);
                                            printf("ERROR: Syntax error: %s %s %s %s \n", command, reg + 1, current_word, label);

                                            return 1;
                                        }
                                    }
                                    

                                    else 
                                    {
                                        fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                                pointer - buffer - sizeof(char),  
                                                count_cmd, cmd , command, reg + 1, current_word, label);
                                        printf("ERROR: Syntax error: %s %s %s %s \n", command, reg + 1, current_word, label);

                                        return 1;
                                    }

                                }
                            }
                        }
                            break;
                            
                        default:
                        {
                            fprintf(listing, "%#8x %8lu %6lu Unknown error %8s  %3s %1s %4s\n",
                                    pointer - buffer - sizeof(char),  
                                    count_cmd, cmd , command, reg + 1, current_word, label);
                            printf("ERROR: Unknown error: %8s %3s %1s %4s \n", command, reg + 1, current_word, label);

                            return 1;
                        }
                        }
                    }
                    
                    else
                    {   
                        int argc = sscanf(current_pointer, " %s %s %s", reg, current_word, label);

                        switch (argc)
                        {
                        case -1:
                        case 0:
                        {
                            if ((cmd == CMD_POP) || (cmd == CMD_PURGE) || (cmd == CMD_IN) || (cmd == CMD_OUT))
                            {
                                *pointer = (char)MOD_EMPTY;
                                ++pointer;

                                fprintf(listing, "%#8x %8lu %5lu %2lu %8s\n",
                                        pointer - buffer - 2 * sizeof(char),
                                        count_cmd, cmd, MOD_EMPTY, command);
                            }

                            else
                            {
                                fprintf(listing, "%#8x %8lu %4lu %1lu this cmd don't have empty mod %8s %s\n",
                                        pointer - buffer - sizeof(char),  
                                        count_cmd, cmd, MOD_EMPTY, command, reg);
                                printf("ERROR: Unknown mod or register: %8s %s \n", command, reg);

                                return 1;
                            }
                            break;
                        }
                            
                        case 1:
                        {
                            sscanf(current_pointer, " %s", reg);
                            Str_uppercase(reg);

                            char number_register = Code_reg(reg);

                            if (number_register != -1)
                            {
                                *pointer = MOD_00R;
                                ++pointer;

                                *pointer = number_register;
                                ++pointer;

                                fprintf(listing, "%#8x %8lu %3lu %1lu %2lu %8s %s\n", 
                                        pointer - buffer - 3 * sizeof(char),
                                        count_cmd, cmd, MOD_00R, number_register, command, reg);
                            }

                            else if (sscanf(reg, "%lg", &value ) == 1)
                            { 
                                *pointer = MOD_0A0;
                                ++pointer;
                                    
                                *((element_t*)pointer) = value;
                                pointer += sizeof(element_t);

                                fprintf(listing, "%#8x %8lu %3lu %1lu %2lg %8s %lg\n", 
                                        pointer - buffer - sizeof(element_t) - 2 * sizeof(char),
                                        count_cmd, cmd, MOD_0A0, value,  command, value);
                            }
                            
                            else
                            {
                                fprintf(listing, "%#8x %8lu %6lu Syntax error %8s %s\n",
                                        pointer - buffer - sizeof(char),  
                                        count_cmd, cmd , command, reg);
                                printf("ERROR: Syntax error: %8s %s \n", command, reg);

                                    return 1;
                            }
                            break;
                        }
                    
                        case 3:
                        {
                            if (current_word[0] != '+')
                            {
                                fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                        pointer - buffer - sizeof(char),  
                                        count_cmd, cmd , command, reg, current_word, label);
                                printf("ERROR: Syntax error: %8s %3s %1s %4s \n", command, reg, current_word, label);

                                return 1;
                            }

                            else
                            {   
                                Str_uppercase(reg);
                                Str_uppercase(label);

                                char number_register = Code_reg(reg);

                                if (number_register != -1)
                                {
                                    if (sscanf(label, "%lg", &value ) == 1)
                                    {
                                        *pointer = MOD_0AR;
                                        ++pointer;

                                        *((element_t*)pointer) = value;
                                        pointer += sizeof(element_t);

                                        *pointer = number_register;
                                        ++pointer;

                                        fprintf(listing, "%#8x %8lu %2lu %1lu %2lg %1lu %8s %3lg %1lu \n", 
                                                pointer - buffer - sizeof(element_t) - 3 * sizeof(char),
                                                count_cmd, cmd, MOD_0AR, value, number_register,  command, value, reg);
                                    }
                                    else
                                    {
                                        fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                                pointer - buffer - sizeof(char),  
                                                count_cmd, cmd , command, reg, current_word, label);
                                        printf("ERROR: Syntax error: %8s %3s %1s %4s \n", command, reg, current_word, label);

                                        return 1;
                                    }
                                }

                                number_register = Code_reg(label);

                                if (number_register != -1)
                                {
                                    if (sscanf(reg, "%lg", &value ) == 1)
                                    {
                                        *pointer = MOD_0AR;
                                        ++pointer;

                                        *((element_t*)pointer) = value;
                                        pointer += sizeof(element_t);

                                        *pointer = number_register;
                                        ++pointer;

                                        fprintf(listing, "%#8x %8lu %2lu %1lu %2lg %1lu %8s %3lg %1lu \n", 
                                            pointer - buffer - sizeof(element_t) - 3 * sizeof(char),
                                            count_cmd, cmd, MOD_0AR, value, number_register,  command, value, label);
                                    }
                                    else
                                    {
                                        fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                                pointer - buffer - sizeof(char),  
                                                count_cmd, cmd , command, reg, current_word, label);
                                        printf("ERROR: Syntax error: %8s %3s %1s %4s \n", command, reg, current_word, label);

                                        return 1;
                                    }
                                }

                                else 
                                {
                                    fprintf(listing, "%#8x %8lu %6lu Syntax error %8s  %3s %1s %4s\n",
                                            pointer - buffer - sizeof(char),  
                                            count_cmd, cmd , command, reg, current_word, label);
                                    printf("ERROR: Syntax error: %8s %3s %1s %4s \n", command, reg, current_word, label);

                                    return 1;
                                }
                            }
                            break;
                        }

                        default:
                        {
                            fprintf(listing, "%#8x %8lu %6lu Unknown error %8s  %3s %1s %4s\n",
                                    pointer - buffer - sizeof(char),  
                                    count_cmd, cmd , command, reg, current_word, label);
                            printf("ERROR: Unknown error: %8s %3s %1s %4s \n", command, reg, current_word, label);

                            return 1;
                        }
                        }
                    }
                }

                else
                {
                    fprintf(listing, "%#8x %8lu %8lu %8s\n", 
                            pointer - buffer - sizeof(char),
                            count_cmd, cmd, command);
                }
            }

            else
            {
                fprintf(listing, "%#8x %8lu Unknown command %8s\n", 
                        pointer - buffer - sizeof(char),
                        count_cmd, command);
                printf("ERROR: Unknown command: %8s\n", command);

                return 1;
            }
        }

        Free_mem(command, reg, label, current_word);
    }

    size = pointer - buffer;

    fwrite(&Version,   sizeof(char),          1, code);
    fwrite(&Signature, sizeof(unsigned char), 1, code);
    fwrite(&size,      sizeof(size_t),        1, code);
    fwrite(&count_cmd, sizeof(size_t),        1, code);
    fwrite(buffer,     sizeof(char),       size, code);

    free(buffer);
    buffer = nullptr;

    return 0;
}

void Free_mem(char* command, char* reg, char* label, char* current_word)
{
    free(command);
    free(reg);
    free(label);
    free(current_word);

    command      = nullptr;
    reg          = nullptr;
    label        = nullptr;
    current_word = nullptr;
}

int Code_cmd(const char* cmd)
{
    assert(cmd != nullptr);

    int hcmd = Hash(cmd);

    #define DEF_COMMAND(name, number, hash, argc, code) if (hcmd == hash) return CMD_##name;
    #define DEF_MOD(name, number)
    #define DEF_REG(name, number, hash)

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 

    /*else*/ return -1;
}

int Code_reg(const char* reg)
{
    assert(reg != nullptr);

    int hreg = Hash(reg);

    #define DEF_COMMAND(name, number, hash, argc, code)
    #define DEF_MOD(name, number) 
    #define DEF_REG(name, number, hash) if (hreg == hash) return REG_##name;

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 

    /*else*/ return -1;  
}

int Arg_command(int cmd)
{
    assert(cmd != -1);

    #define DEF_COMMAND(name, number, hash, argc, code) if ((cmd == CMD_##name) && (argc >= 1)) return 1;
    #define DEF_MOD(name, number) 
    #define DEF_REG(name, number, hash)

    #include "../libr/command.h"

    #undef DEF_COMMAND
    #undef DEF_MOD
    #undef DEF_REG 

    /*else*/ return 0;
}

void Str_uppercase(char* string)
{
    assert(string != nullptr);

    int len = strlen(string);

    for (int i = 0; i < len; ++i)
    {
        string[i] = toupper(string[i]);
    }
}

unsigned int Hash(const char* cmd)
{
    assert(cmd != nullptr);

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

int Next_word(char* begin, char* word, size_t* lshift, size_t* rshift)
{
    assert(begin  != nullptr);
    assert(word   != nullptr);
    assert(lshift != nullptr);
    assert(rshift != nullptr);

    *lshift = 0;
    *rshift = 0;

    sscanf(begin, " %n%s%n", lshift, word, rshift);

    if (*rshift == 0) 
    {
        return 0;
    }

    else 
    {
        return 1;
    }
}

