#ifndef BACKENDH
#define BACKENDH

//=============================================================================================================

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"

//=============================================================================================================

const int MAX_SIZE_COMMAND        = 400;
const size_t INITIAL_CAPACITY     = 20;

//=============================================================================================================

struct RAM
{
    size_t bp;

    size_t count_ifjmp;

    struct Stack* locale_var;
    struct Stack* global_var;

};

struct Variable
{
    char* name;
    unsigned int hash;

    size_t number;
    int initialization;
};

enum KEY_WORDS
{
    #define KEY_WORD(replace, len, word, type, value, number, path) KEY##_##number = value,

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD
};

//=============================================================================================================

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program);

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line);

char* Get_name_file(const char* file);

char* Get_word(char* begin);

size_t Skip_separator(char** string);

char* Name_key_word(char* str);

int Type_key_word(char* str);

double Code_key_word(char* str);

int Is_key_word(char* str);

//=====================================================================================================================

void Generate_ASM_code(struct Tree* tree);

void RAM_construct(struct RAM* ram);

void RAM_destruct(struct RAM* ram);

void Free_vars(struct Stack* vars);

struct Variable* Get_var(struct Stack* vars, size_t index);

void Print_begin(struct Tree* tree, struct RAM* ram, FILE* code);

void Print_dec(struct Tree* tree, struct RAM* ram, FILE* code);

void Print_func(struct Tree* tree, struct Node* current_node, struct RAM* ram, FILE* code);

void Print_body(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_op(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_print(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_scan(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_if(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_while(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_assign(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_call(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_ret(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_arg(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_equation(struct Tree* tree, struct Node* current_node, struct RAM* ram, size_t count_var, FILE* code);

void Print_and_find_global_var(struct Tree* tree, struct RAM* ram, FILE* code);

int Find_main(struct Tree* tree);

void Find_locale_var(struct Tree* tree, struct Node* current_node, struct Stack* locale_vars, struct Stack* global_vars);

struct Variable* Search_var(struct Stack* vars, char* name_var);

struct Variable* Variable_create_and_fill(char* name, unsigned int hash, size_t number, int initialization);

unsigned int Hash(const char* cmd);

#endif