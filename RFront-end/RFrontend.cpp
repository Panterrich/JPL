#include "RFrontend.h"

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(text != nullptr);

    size_t number_line = 0;

    tree->name_equation = Get_name_file(name_program);
    tree->root = Node_create(tree, nullptr, text, &number_line);
}

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line)
{
    char* begin = (text->lines)[*number_line].str;
    Skip_separator(&begin);

    if (*begin == '{')
    {
        ++(*number_line);
        begin = (text->lines)[*number_line].str;
        Skip_separator(&begin);

        char* word = Get_word(begin);
        
        if (strcmp(word, "nil") == 0)
        {
            free(word);

            ++(*number_line);
            begin = (text->lines)[*number_line].str;
            Skip_separator(&begin);

            if (*begin == '}') ++(*number_line);
            else
            {
                printf("Syntax error: line #%d\n", *number_line);
            }

            return nullptr;
        }

        else 
        {   
            struct Node* current_node = nullptr;

            if (strcmp(word, "DEC") == 0)
            {
                #ifdef HG
                    char* op = strdup("やれやれだぜ");
                #else 
                    char* op = strdup("Yare Yare Daze");
                #endif

                current_node = Node_create_and_fill(nullptr, 
                                                    DECLARATE, op, NAN, previous_node, 
                                                    nullptr);

                free(word);
            }

            else if (strcmp(word, "LR") == 0)
            {
                #ifdef HG
                    char* op = strdup("やれやれだぜ");
                #else 
                    char* op = strdup("Yare Yare Daze");
                #endif

                current_node = Node_create_and_fill(nullptr, 
                                                    LR, op, NAN, previous_node, 
                                                    nullptr);
                free(word);
            }

            else if (word[0] == '$')
            {   
                current_node = Node_create_and_fill(nullptr, 
                                                    FUNC, Name_key_word(word + 1), Code_key_word(word + 1), previous_node, 
                                                    nullptr);
                free(word);
            }

            else if ((word[0] == '-' && isdigit(word[1])) || isdigit(word[0]))
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    NUMBER, nullptr, strtod(begin, &begin), previous_node,
                                                    nullptr);
                free(word);
            }

            else if (Is_key_word(word))
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    Type_key_word(word), Name_key_word(word), Code_key_word(word), previous_node,
                                                    nullptr);
                
                free(word);
            }

            else 
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    VAR, word, NAN, previous_node,
                                                    nullptr);
            }

            ++(*number_line);
            ++(tree->size);

            current_node->left  = Node_create(tree, current_node, text, number_line);
            current_node->right = Node_create(tree, current_node, text, number_line);

            begin = (text->lines)[*number_line].str;
            Skip_separator(&begin);

            if (*begin == '}') ++(*number_line);
            else
            {
                printf("Syntax error: line #%d\n", *number_line);
            }

            return current_node;
        }
    }

    return nullptr;
}

char* Get_name_file(const char* file)
{
    assert(file != nullptr);

    char* name_file   = strdup(file);
    char* pointer_format  = strchr(name_file, '.');
    if   (pointer_format != nullptr) *pointer_format = '\0';

    return name_file;
}

char* Get_word(char* begin)
{
    char* current_symbol = begin;

    while (!isspace(*current_symbol) && *current_symbol != '\0' && *current_symbol != '\n') ++current_symbol;

    return strndup(begin, current_symbol - begin); 
}

size_t Skip_separator(char** string)
{
    assert(string != nullptr);

    size_t count = 0;

    while ((isspace((int)(unsigned char)**string) || **string == '\n') && **string != '\0')
    {
        ++(*string);
        ++count;
    }

    return count; 
}

char* Name_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return strdup(replace);

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return strdup(str);
}

int Type_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return type;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return 0;
}

double Code_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return value;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return NAN;
}

int Is_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return 1;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return 0;
}

void Tree_print(struct Tree* tree)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    char name_output[MAX_SIZE_COMMAND] = {};
    sprintf(name_output, "%s_out.txt", tree->name_equation);

    FILE* file = fopen(name_output, "w");
    
    size_t tab = 0;
    Print_dec(tree->root, &tab, file);

    fclose(file);
}

void Print_dec(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    if (current_node == nullptr) return;

    if (current_node->type == DECLARATE)
    {
        if (current_node->left->type == FUNC) 
        {
            Print_func(current_node->left, tab, file);
        }

        else 
        {
            Print_assign(current_node->left, tab, file);
            fprintf(file, "\n");
        }
       
        Print_dec(current_node->right, tab, file);
    }

    else
    {
        printf("[%p] : type %d str %s value %lg", current_node, 
                                                  current_node->type, 
                                                  current_node->str, 
                                                  current_node->value);
    }
}

void Print_func(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    if ((current_node->type == FUNC) && (current_node->prev->type == DECLARATE))
    {
        tprintf(file, "%s (", current_node->str);

        if (current_node->left != nullptr) 
        {
            fprintf(file, "%s", current_node->left->str);
            Print_param(current_node->left->left, file);
        }

        fprintf(file, ")\n");

        tprintf(file, "Za Warudo! Toki wo Tomare!\n");
        ++(*tab);
        Print_body(current_node->right, tab, file);
        --(*tab);
        tprintf(file, "ROAD ROLLER DA!\n\n\n");
    }
}

void Print_param(struct Node* current_node, FILE* file)
{
    assert(file != nullptr);
    if (current_node == nullptr) return;

    fprintf(file, ", %s", current_node->str);

    Print_param(current_node->left, file);
}

void Print_body(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    if (current_node->type == LR) 
    {
        Print_op(current_node->left, tab, file);
        Print_body(current_node->right, tab, file);
    }
}

void Print_op(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);

    if (current_node == nullptr) return;

    if (current_node->type == LR && current_node->value == 3)
    {
        return;
    }

    if (current_node->type == FUNC && current_node->value == 10)
    {
        Print_print(current_node, tab, file);
        return;
    }

    if (current_node->type == FUNC && current_node->value == 11)
    {
        Print_scan(current_node, tab, file);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 30)
    {   
        Print_if(current_node, tab, file);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 31)
    {
        Print_while(current_node, tab, file);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 32)
    {
        Print_assign(current_node, tab, file);
        return;
    }

    if (current_node->type == FUNC)
    {
        Print_call(current_node, tab, file);
        return;
    }

    
    Print_ret(current_node, tab, file);
    return;
}

void Print_print(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    tprintf(file, "%s ", current_node->str);
    Print_equation(current_node->left, current_node->left, file);
    fprintf(file, " %s\n", current_node->prev->str);

}

void Print_scan(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    tprintf(file, "%s ", current_node->str);
    Print_equation(current_node->left, current_node->left, file);
    fprintf(file, " %s\n", current_node->prev->str);
}

void Print_if(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    fprintf(file, "\n");
    tprintf(file, "%s (", current_node->str);
    Print_equation(current_node->left, current_node->left, file);
    fprintf(file, ")\n");

    if (current_node->right->right == nullptr)
    {
        Print_body(current_node->right, tab, file);
        fprintf(file, "\n");
    }

    else
    {
        tprintf(file, "Za Warudo! Toki wo Tomare!\n");
        ++(*tab);
        Print_body(current_node->right, tab, file);
        --(*tab);
        tprintf(file, "ROAD ROLLER DA!\n\n");
    }
}

void Print_while(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    fprintf(file, "\n");
    tprintf(file, "%s (", current_node->str);
    Print_equation(current_node->left, current_node->left, file);
    fprintf(file, ")\n");

    if (current_node->right->right == nullptr)
    {
        Print_body(current_node->right, tab, file);
        fprintf(file, "\n");
    }

    else
    {
        tprintf(file, "Za Warudo! Toki wo Tomare!\n");
        ++(*tab);
        Print_body(current_node->right, tab, file);
        --(*tab);
        tprintf(file, "ROAD ROLLER DA!\n\n");
    }
}

void Print_assign(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    tprintf(file, "%s %s ", current_node->left->str, current_node->str);
    Print_equation(current_node->right, current_node->right, file);
    fprintf(file, " %s\n", current_node->prev->str);
}

void Print_call(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    tprintf(file, "%s(", current_node->str);

    if (current_node->left != nullptr) 
    {   
        Print_arg(current_node->left, file);
    }

    fprintf(file, ") %s\n", current_node->prev->str);
}

void Print_arg(struct Node* current_node, FILE* file)
{
    assert(file != nullptr);
    if (current_node == nullptr) return;

    if (current_node->type == LR)
    {
        if (current_node->right != nullptr) Print_arg(current_node->right, file);

        if (current_node->left != nullptr) 
        {
            fprintf(file, ", ");
            Print_arg(current_node->left, file);
        }
    }

    else
    {  
       Print_equation(current_node, current_node, file);
    }
}

void Print_ret(struct Node* current_node, size_t* tab, FILE* file)
{
    assert(file != nullptr);
    assert(tab  != nullptr);
    if (current_node == nullptr) return;

    Print_tab(tab, file);
    Print_equation(current_node->left, current_node->left, file);
    fprintf(file, " %s %s\n", current_node->str, current_node->prev->str);
}

void Print_equation(struct Node* current_node, struct Node* begin, FILE* file)
{
    assert(file != nullptr);
    if (current_node == nullptr) return;

    switch (current_node->type)
    {
        case VAR: fprintf(file, "%s", current_node->str);  break;
        case NUMBER:
        {
            if ((current_node->value < 0) && (current_node != begin))
            {
                fprintf(file, "(%lg)", current_node->value);
            }

            else
            {
                fprintf(file, "%lg", current_node->value);
            }

            break;
        }

        case FUNC:
        {
            fprintf(file, "%s(", current_node->str);

            if (current_node->left != nullptr) 
            {
                Print_arg(current_node->left, file);
            }
        
            fprintf(file, ")");
            break;
        }

        case OPERATION:
        {
            if (current_node->value == 52)
            {
                fprintf(file, "(");
                Print_equation(current_node->right, current_node->right, file);
                fprintf(file, ") %s %s", current_node->str, current_node->left->str);
            }

            else if (current_node->value >= 46 && current_node->value <= 51)
            {
                Print_equation(current_node->left, current_node->left, file);
                fprintf(file, " %s ", current_node->str);
                Print_equation(current_node->right, current_node->right, file);
            }

            else if (current_node->value >= 41 && current_node->value <= 44)
            {
                if (current_node == begin                  || (current_node->prev->type == OPERATION && current_node->prev->value == 45)  ||
                   (current_node->prev->type == FUNC       && (current_node->prev->value == 78       || current_node->prev->value == 79)) ||
                   (current_node->prev->type == OPERATION  && (current_node->prev->value == 41       || current_node->prev->value == 42)  &&
                   (current_node->type       == OPERATION) && (current_node->value == 41             || current_node->value == 42)))
                {
                    Print_equation(current_node->left, begin, file);
                    fprintf(file, " %s ", current_node->str);
                    Print_equation(current_node->right, begin, file);
                }

                else
                {
                    fprintf(file, "(");
                    Print_equation(current_node->left, begin, file);
                    fprintf(file, " %s ", current_node->str);
                    Print_equation(current_node->right, begin, file);
                    fprintf(file, ")");
                }
            }

            else
            {
                if (current_node->left->type == OPERATION)
                {
                    fprintf(file, "(");
                    Print_equation(current_node->left, begin, file);
                    fprintf(file, ") %s ", current_node->str);
                }

                else 
                {
                    Print_equation(current_node->left, begin, file);
                    fprintf(file, " %s ", current_node->str);
                }

                    
                if (current_node->right->type == OPERATION)
                {
                    fprintf(file, "(");
                    Print_equation(current_node->right, begin, file);
                    fprintf(file, ")");
                }

                else
                {
                    Print_equation(current_node->right, begin, file);
                }
            }

            break;
        }
            
        default:
            printf("ERROR: %s", current_node->str);
            break;
    }
}

void Print_tab(size_t* tab, FILE* file)
{
    for (size_t index = 0; index < *tab; ++index)
    {
        fprintf(file, "\t");
    }
}