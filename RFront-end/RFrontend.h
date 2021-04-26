#ifndef RFRONTENDH
#define RFRONTENDH

#include "../libr/Tree/Tree.h"
#include "../libr/Onegin/Onegin.h"
#include <locale.h>

//=========================================================================================================

const int INITIAL_VAR = 20;
const size_t MAX_SIZE_COMMAND = 300;

//=========================================================================================================

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

//=========================================================================================================

#define strlen_n(s) ((s) == nullptr ? 0 : strlen(s))
#define tprintf(file, ...) Print_tab(tab, file); fprintf(file, __VA_ARGS__);

//=========================================================================================================

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program);

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line);

char* Get_name_file(const char* file);

char* Get_word(char* begin);

size_t Skip_separator(char** string);

char* Name_key_word(char* str);

int Type_key_word(char* str);

double Code_key_word(char* str);

int Is_key_word(char* str);

//=========================================================================================================

void Tree_print(struct Tree* tree);

void Print_dec(struct Node* current_node, size_t* tab, FILE* file);

void Print_func(struct Node* current_node, size_t* tab,  FILE* file);

void Print_param(struct Node* current_node, FILE* file);

void Print_body(struct Node* current_node, size_t* tab, FILE* file);

void Print_op(struct Node* current_node, size_t* tab, FILE* file);

void Print_print(struct Node* current_node, size_t* tab, FILE* file);

void Print_scan(struct Node* current_node, size_t* tab, FILE* file);

void Print_if(struct Node* current_node, size_t* tab, FILE* file);

void Print_while(struct Node* current_node, size_t* tab, FILE* file);

void Print_assign(struct Node* current_node, size_t* tab, FILE* file);

void Print_call(struct Node* current_node, size_t* tab, FILE* file);

void Print_arg(struct Node* current_node, FILE* file);

void Print_ret(struct Node* current_node, size_t* tab, FILE* file);

void Print_equation(struct Node* current_node, struct Node* begin, FILE* file);

void Print_tab(size_t* tab, FILE* file);

#endif