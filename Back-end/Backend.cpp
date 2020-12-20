#include "Backend.h"
//==================================================================================================================

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
                current_node = Node_create_and_fill(nullptr, 
                                                    DECLARATE, word, NAN, previous_node, 
                                                    nullptr);
            }

            else if (strcmp(word, "LR") == 0)
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    LR, word, NAN, previous_node, 
                                                    nullptr);
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

    const char* begin = file;
    while ((strchr(begin, '/')) != nullptr) begin = strchr(begin, '/') + 1;
    char* name_file   = strdup(begin);

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
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return strdup(word);

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

void Generate_ASM_code(struct Tree* tree)
{
    Tree_null_check(tree);

    char name_output[MAX_SIZE_COMMAND] = {};
    sprintf(name_output, "files/%s.asm", tree->name_equation);

    FILE* code = fopen(name_output, "w");

    if (!Find_main(tree)) 
    {
        tree->error = NO_MAIN_DECLARATION;
        TREE_ASSERT_OK(tree);
    }

    struct RAM ram = {};
    RAM_construct(&ram);

    Print_begin(tree, &ram, code);
    Print_dec(tree, &ram, code);

    Free_vars(ram.global_var);

    fclose(code);
}

void RAM_construct(struct RAM* ram)
{
    assert(ram != nullptr);

    ram->bp = 0;
    ram->count_ifjmp = 0;
    ram->locale_var = (struct Stack*) calloc(1, sizeof(struct Stack));
    ram->global_var = (struct Stack*) calloc(1, sizeof(struct Stack));

    STACK_CONSTRUCT(ram->locale_var, INITIAL_CAPACITY);
    STACK_CONSTRUCT(ram->global_var, INITIAL_CAPACITY);
}

void RAM_destruct(struct RAM* ram)
{
    assert(ram != nullptr);

    ram->bp = 0;
    ram->count_ifjmp = 0;

    Stack_destruct(ram->locale_var);
    Stack_destruct(ram->global_var);

    free(ram->locale_var);
    free(ram->global_var);

    ram->locale_var = nullptr;
    ram->global_var = nullptr;
}

void Free_vars(struct Stack* vars)
{
    while (vars->size > 0)
    {
        struct Variable* var = (struct Variable*) Stack_pop(vars);

        free(var->name);
        free(var);
    }
}

struct Variable* Get_var(struct Stack* vars, size_t index) 
{
    assert(vars != nullptr);

    return ((struct Variable*)((vars->data)[index]));
}

void Print_begin(struct Tree* tree, struct RAM* ram, FILE* code)
{
    assert(code != nullptr);

    Print_and_find_global_var(tree, ram, code);

    fprintf(code, "\n\n"
                  "push %lu\n"
                  "pop rfx\n"
                  "call :main\n"
                  "hlt\n\n\n", ram->global_var->size);
}

void Print_dec(struct Tree* tree, struct RAM* ram, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    struct Node* current_node = tree->root;

    while (current_node->type == DECLARATE)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == FUNC) Print_func(tree, current_node->left, ram, code);
        }

        current_node = current_node->right;

        if (current_node == nullptr) break;
    }

    if (current_node != nullptr)
    {   
        if (current_node->type == FUNC) Print_func(tree, current_node, ram, code);
    }
}

void Print_func(struct Tree* tree, struct Node* current_node, struct RAM* ram, FILE* code)
{
    Tree_null_check(tree);
    assert(code != nullptr);
    assert(ram  != nullptr);

    if (current_node == nullptr) return;

    Find_locale_var(tree, current_node->left, ram->locale_var, ram->global_var);
    size_t count_param = ram->locale_var->size;

    for (size_t index = 0; index < ram->locale_var->size; ++index)
    {
        Get_var(ram->locale_var, index)->initialization = 1;
    }

    Find_locale_var(tree, current_node->right, ram->locale_var, ram->global_var);
    size_t count_var = ram->locale_var->size;

    fprintf(code, "%s:\n", current_node->str);

    for (size_t index = 0; index < count_param; ++index)
    {
        fprintf(code, "pop [rfx + %lu]\n", index);
    }

    fprintf(code, "\n");

    Print_body(tree, current_node->right, ram, count_var, code);
    fprintf(code, "ret\n\n\n");

    Free_vars(ram->locale_var);
}

void Print_body(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;

    if (current_node->type == LR) 
    {
        Print_op(tree, current_node->left, ram, count_var, code);
        Print_body(tree, current_node->right, ram, count_var, code);
    }
}

void Print_op(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;

    if (current_node->type == LR && current_node->value == 3)
    {
        return;
    }

    if (current_node->type == FUNC && current_node->value == 10)
    {
        Print_print(tree, current_node, ram, count_var, code);
        return;
    }

    if (current_node->type == FUNC && current_node->value == 11)
    {
        Print_scan(tree, current_node, ram, count_var, code);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 30)
    {   
        Print_if(tree, current_node, ram, count_var, code);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 31)
    {
        Print_while(tree, current_node, ram, count_var, code);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 32)
    {
        Print_assign(tree, current_node, ram, count_var, code);
        return;
    }

    if (current_node->type == FUNC)
    {
        Print_call(tree, current_node, ram, count_var, code);
        return;
    }

    
    Print_ret(tree, current_node, ram, count_var, code);
    return;
}

void Print_print(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;

    Print_equation(tree, current_node->left, ram, count_var, code);

    fprintf(code, "out\n");
    
}

void Print_scan(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;

    struct Variable* var = Search_var(ram->global_var, current_node->left->str);
    
    if (var != nullptr)
    {
        fprintf(code, "in\n"
                      "pop [%lu]\n", var->number);
    }

    else
    {
        var = Search_var(ram->locale_var, current_node->left->str);

        if (var == nullptr)
        {
            tree->error = VAR_NOT_FOUND;
            TREE_ASSERT_OK(tree);
        }

        var->initialization = 1;

        fprintf(code, "in\n"
                      "pop [rfx + %lu]\n", var->number);
    }
}

void Print_if(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    fprintf(code, "\n"
                  "If_begin%lu:\n", ram->count_ifjmp);
    Print_equation(tree, current_node->left->left, ram, count_var, code);
    Print_equation(tree, current_node->left->right, ram, count_var, code);

    if (current_node->left->type == OPERATION && current_node->left->value >= 45 && current_node->left->value <= 50)
    {
        switch ((int)current_node->left->value)
        {
            case 46/* >  */: fprintf(code, "jbe :If_end%lu\n\n",  ram->count_ifjmp); break;
            case 47/* <  */: fprintf(code, "jae :If_end%lu\n\n",  ram->count_ifjmp); break;
            case 48/* == */: fprintf(code, "jne :If_end%lu\n\n",  ram->count_ifjmp); break;
            case 49/* >= */: fprintf(code, "jb :If_end%lu\n\n", ram->count_ifjmp); break;
            case 50/* <= */: fprintf(code, "ja :If_end%lu\n\n", ram->count_ifjmp); break;
            case 51/* != */: fprintf(code, "je :If_end%lu\n\n", ram->count_ifjmp); break;
            
            default: break;
        }
    }

    else
    {
        tree->error = WRONG_CONDITION;
        TREE_ASSERT_OK(tree);
    }

    if (current_node->right->type == LR) Print_body(tree, current_node->right, ram, count_var, code);
    else                                   Print_op(tree, current_node->right, ram, count_var, code);
    
    fprintf(code, "\n"
                  "If_end%lu:"
                  "\n\n", ram->count_ifjmp);
    ++(ram->count_ifjmp);
}

void Print_while(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    fprintf(code, "\n"
                  "While_begin%lu:\n", ram->count_ifjmp);
    Print_equation(tree, current_node->left->left,  ram, count_var, code);
    Print_equation(tree, current_node->left->right, ram, count_var, code);

    if (current_node->left->type == OPERATION && current_node->left->value >= 45 && current_node->left->value <= 50)
    {
        switch ((int)current_node->left->value)
        {
            case 46/* >  */: fprintf(code, "jbe :While_end%lu\n\n",  ram->count_ifjmp); break;
            case 47/* <  */: fprintf(code, "jae :While_end%lu\n\n",  ram->count_ifjmp); break;
            case 48/* == */: fprintf(code, "jne :While_end%lu\n\n",  ram->count_ifjmp); break;
            case 49/* >= */: fprintf(code, "jb :While_end%lu\n\n", ram->count_ifjmp); break;
            case 50/* <= */: fprintf(code, "ja :While_end%lu\n\n", ram->count_ifjmp); break;
            case 51/* != */: fprintf(code, "je :While_end%lu\n\n", ram->count_ifjmp); break;
            
            default: break;
        }
    }

    else
    {
        tree->error = WRONG_CONDITION;
        TREE_ASSERT_OK(tree);
    }

    if (current_node->right->type == LR) Print_body(tree, current_node->right, ram, count_var, code);
    else Print_op(tree, current_node->right, ram, count_var, code);
    
    fprintf(code, "\n"
                  "jmp :While_begin%lu\n" 
                  "While_end%lu:\n\n", ram->count_ifjmp, ram->count_ifjmp);
    ++(ram->count_ifjmp);
}

void Print_assign(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    struct Variable* var = Search_var(ram->global_var, current_node->left->str);

    if (var != nullptr)
    {
        Print_equation(tree, current_node->right, ram, count_var, code);

        fprintf(code, "pop [%lu]\n", var->number);
    }

    else 
    {
        struct Variable* var = Search_var(ram->locale_var, current_node->left->str);

        if (var == nullptr)
        {
            tree->error = VAR_NOT_FOUND;
            TREE_ASSERT_OK(tree);
        }

        var->initialization  = 1;

        Print_equation(tree, current_node->right, ram, count_var, code);

        fprintf(code, "pop [rfx + %lu]\n", var->number);
    }
}

void Print_call(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    fprintf(code, "\n\n");

    Print_arg(tree, current_node->left, ram, count_var, code);

    fprintf(code, "\n"
                  "push %lu\n"
                  "push rfx\n"
                  "add\n"
                  "pop rfx\n"
                  "\n"
                  "call :%s\n"
                  "\n"
                  "push rfx\n"
                  "push %lu\n"
                  "sub\n"
                  "pop rfx\n"
                  "\n\n", count_var, current_node->str, count_var);
}

void Print_ret(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    Print_equation(tree, current_node->left, ram, count_var, code);

    fprintf(code, "ret\n");
}

void Print_arg(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;

    if (current_node->type == LR)
    {
        if (current_node->left != nullptr)  Print_arg(tree, current_node->left,  ram, count_var, code);
        if (current_node->right != nullptr) Print_arg(tree, current_node->right, ram, count_var, code);
    }

    else
    {  
        Print_equation(tree, current_node, ram, count_var, code);
        fprintf(code, "\n");
    }
}

void Print_equation(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    if (current_node == nullptr) return;
    
    switch (current_node->type)
    {
        case NUMBER: fprintf(code, "push %lg\n", current_node->value); break;
        case VAR:
        {
            struct Variable* var = Search_var(ram->global_var, current_node->str);

            if (var != nullptr)
            {
                fprintf(code, "push [%lu]\n", var->number);
            }

            else 
            {
                struct Variable* var = Search_var(ram->locale_var, current_node->str);

                if (var == nullptr)
                {
                    tree->error = VAR_NOT_FOUND;
                    TREE_ASSERT_OK(tree);
                }

                if (var->initialization == 0)
                {
                    *(int*)0 = 0;
                    // tree->error = NO_INITIALIZATION_VAR;
                    // TREE_ASSERT_OK(tree);
                }

                fprintf(code, "push [rfx + %lu]\n", var->number);
            }
            
            break;
        }

        case OPERATION:
        {
            Print_equation(tree, current_node->left, ram, count_var, code);
            Print_equation(tree, current_node->right, ram, count_var, code);

            switch ((int)current_node->value)
            {
                case 41: fprintf(code, "ADD\n"); break;
                case 42: fprintf(code, "SUB\n"); break;
                case 43: fprintf(code, "MUL\n"); break;
                case 44: fprintf(code, "DIV\n"); break;
                case 45: fprintf(code, "POW\n"); break;
                case 46: fprintf(code, "BAA\n"); break;
                case 47: fprintf(code, "BAB\n"); break;
                case 48: fprintf(code, "BAE\n"); break;
                case 49: fprintf(code, "BAAE\n"); break;
                case 50: fprintf(code, "BABE\n"); break;
                case 51: fprintf(code, "BANE\n"); break;
                
                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }

            break;
        }

        case FUNC:
        {
            if (current_node->value >= 65 && current_node->value <= 79)
            {
                Print_arg(tree, current_node->left, ram, count_var, code);
            }

            switch ((int)current_node->value)
            {
                case 65: fprintf(code, "LN\n");     break;
                case 66: fprintf(code, "SIN\n");    break;
                case 67: fprintf(code, "COS\n");    break;
                case 68: fprintf(code, "TG\n");     break;
                case 69: fprintf(code, "CTG\n");    break;
                case 70: fprintf(code, "ARCSIN\n"); break;
                case 71: fprintf(code, "ARCCOS\n"); break;
                case 72: fprintf(code, "ARCTG\n");  break;
                case 73: fprintf(code, "ARCCTG\n"); break;
                case 74: fprintf(code, "SH\n");     break;
                case 75: fprintf(code, "CH\n");     break;
                case 76: fprintf(code, "TH\n");     break;
                case 77: fprintf(code, "CTH\n");    break;
                case 78: fprintf(code, "SQRT\n");   break;
                case 79: fprintf(code, "EXP\n");    break;
                
                default:
                    Print_call(tree, current_node, ram, count_var, code);
                    break;
            }

            break;
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }
}

int Find_main(struct Tree* tree)
{
    Tree_null_check(tree);

    struct Node* current_node = tree->root;

    while (current_node->type == DECLARATE && current_node->right != nullptr)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == FUNC && current_node->left->value == 1) return 1;
            else current_node = current_node->right;
        }
        else current_node = current_node->right;
    }

    if (current_node == nullptr) return 0;

    if (current_node->type == FUNC && current_node->value == 1) return 1;
    else return 0;
}

void Find_locale_var(struct Tree* tree, struct Node* current_node, struct Stack* locale_vars, struct Stack* global_vars)
{
    Tree_null_check(tree);
    assert(locale_vars != nullptr);
    assert(global_vars != nullptr);

    if (current_node == nullptr) return;
    
    if (current_node->type == VAR)
    {
        if (!Search_var(global_vars, current_node->str) && !Search_var(locale_vars, current_node->str))
        {
            struct Variable* new_var = Variable_create_and_fill(current_node->str, Hash(current_node->str), locale_vars->size, 0);
            Stack_push(locale_vars, new_var);
        }
    }

    if (current_node->left  != nullptr) Find_locale_var(tree, current_node->left,  locale_vars, global_vars);
    if (current_node->right != nullptr) Find_locale_var(tree, current_node->right, locale_vars, global_vars);
}

void Print_and_find_global_var(struct Tree* tree, struct RAM* ram, FILE* code)
{
    Tree_null_check(tree);
    assert(ram  != nullptr);
    assert(code != nullptr);

    struct Node* current_node = tree->root;

    while (current_node->type == DECLARATE)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == OPERATION && current_node->left->value == 32)
            {
                struct Node* new_var = current_node->left->left;
                struct Variable* var = Search_var(ram->global_var, new_var->str);

                if (var == nullptr)
                {
                    var = Variable_create_and_fill(new_var->str, Hash(new_var->str), ram->global_var->size, 1);
                    Stack_push(ram->global_var, var);
                }

                Print_equation(tree, current_node->left->right, ram, 0, code);
                fprintf(code, "pop [%lu]\n", var->number);
            }
        }

        current_node = current_node->right;

        if (current_node == nullptr) break;
    }

    if (current_node != nullptr)
    {   
        if (current_node->type == OPERATION && current_node->value == 32)
        {
            struct Node* new_var = current_node->left;
            struct Variable* var = Search_var(ram->global_var, new_var->str);

            if (var == nullptr)
            {
                var = Variable_create_and_fill(new_var->str, Hash(new_var->str), ram->global_var->size, 1);
                Stack_push(ram->global_var, var);
            }

            Print_equation(tree, current_node->right, ram, 0, code);
            fprintf(code, "pop [%lu]\n", var->number);
        }
    }
}

struct Variable* Search_var(struct Stack* vars, char* name_var)
{
    assert(vars != nullptr);
    assert(name_var != nullptr);

    unsigned int hash_var = Hash(name_var);

    for (size_t index = 0; index < vars->size; ++index)
    {
        if (hash_var == Get_var(vars, index)->hash) 
        {
            return Get_var(vars, index);
        }
    }

    return nullptr;
}

struct Variable* Variable_create_and_fill(char* name, unsigned int hash, size_t number, int initialization)
{
    struct Variable* var = (struct Variable*) calloc(1, sizeof(struct Variable));

    var->name           = strdup(name);
    var->hash           = hash;
    var->number         = number;
    var->initialization = initialization;

    return var;
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