#ifndef MIDDLEENDH
#define MIDDLEENDH

//=============================================================================================================
#define ALLDIF
//=============================================================================================================

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"
#include "../libr/Differentiator/Differentiator.h"

//=============================================================================================================

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

void Tree_print(struct Tree* tree);

void Node_print(struct Node* current_node, FILE* file);

struct Node* Tree_search_node(struct Tree* tree, struct Node* current_node, int type, int number);

void Tree_processing(struct Tree* tree);

void All_differentiation(struct Tree* tree, struct Node* current_node);

struct Node* My_derivative(struct Tree* tree, struct Node* current_node);

#endif