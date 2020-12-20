#ifndef FRONTENDH
#define FRONTENDH

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"

//================================================================================================================

struct Program
{
    char* buffer; 
    char* current_symbol;
};

//================================================================================================================

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

//================================================================================================================

const size_t INITIAL_CAPACITY = 20;
const size_t MAX_SIZE_COMMAND = 300;

//================================================================================================================

void Program_create(FILE* file, struct Program* program);

void Tokenizer(struct Program* program, struct Stack* nodes);

size_t Skip_separator(char** string);

void Program_destruct(struct Program* program);

void Free_nodes(struct Stack* nodes);

char* Get_name_file(const char* file);

//================================================================================================================

struct Node* Get_node(struct Stack* nodes, size_t index);

struct Node* GetProg(struct Tree* tree, struct Stack* nodes, const char* name_program);

struct Node* GetDec(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetGlobal(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetFunc(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetParam(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node);

struct Node* GetBody(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetOp(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetPrint(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetScan(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetIf(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetWhile(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetAssign(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetRet(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetCall(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetC(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetE(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetT(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetD(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetP(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetN(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetW(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetGroup(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetArg(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

void Require(struct Tree* tree, struct Stack* nodes, size_t* index_node, int type, double value);

void Syntax_error(struct Stack* nodes, size_t index_node);

//=============================================================================================================

void Tree_print(struct Tree* tree);

void Node_print(struct Node* current_node, FILE* file);

const char* Get_path(int code);

#endif