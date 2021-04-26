#ifndef DIFFERETIATORH
#define DIFFERETIATORH

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <math.h>

//============================================================

#include "../Onegin/Onegin.h"
#include "../Stack/Guard.h"
#include "../Tree/Tree.h"

//============================================================

const int MAX_SIZE_STR            = 200;
const int MAX_SIZE_COMMAND        = 400;
const int MAX_SIZE_NAME_VARIABLES = 20;
const int MAX_SIZE_NAME_FUNC      = 10;

const double ACCURACY = 1E-7;

//============================================================

#define strlen_n(s) ((s) == nullptr ? 0       : strlen(s))
#define strdup_n(s) ((s) == nullptr ? nullptr : strdup(s))

#define PREV_OP(node)   (((node)->prev) == nullptr ? 0 : (char)(node)->prev->value)
#define PREV_TYPE(node) (((node)->prev) == nullptr ? 0 : (char)(node)->prev->type)

//======================================================================

enum OPERATION
{
    #define DEF_OP(operator, OP, number, code) OP = number,
    #define DEF_FUNC(func, FUNC, hash, number, code) 

    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC
};

enum FUNC
{
    #define DEF_OP(opetation, OP, number, code) 
    #define DEF_FUNC(func, FUNC, hash, number, code) FUNC = number, 
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC
};

//======================================================================

struct Node* Derivative(struct Tree* tree, struct Node* current_node, char* var, unsigned int hash_var);
                                                   
double Evaluation(struct Tree* tree, struct Node* current_node);

void Constant_folding(struct Tree* tree, struct Node* current_node);

void Neutral_delete(struct Tree* tree, struct Node* current_node);

void Pow_folding(struct Tree* tree, struct Node* current_node);

void Optimization(struct Tree* tree);

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete);

void Replace_func(struct Tree* tree, struct Node* current_node, double value);

//==========================================================================

size_t Skip_spaces(char** string);

int Code_func(char* func);

char* Name_op(int number_op);

int Subtree_is_number(struct Tree* tree, struct Node* current_node);

size_t Size_subtree(struct Tree* tree, struct Node* current_node);

void Node_count(struct Tree* tree, struct Node* current_node, size_t* count);

int Is_equal(double value, double number);

unsigned int Hash(const char* cmd);


#endif