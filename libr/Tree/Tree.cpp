#include "Tree.h"
#include "../Stack/Guard.h"

//=========================================================================================

void Tree_construct(struct Tree* tree)
{
    Tree_null_check(tree);

    tree->size          = 0;
    tree->error         = 0;
    tree->name_equation = nullptr;
    tree->root          = nullptr;

    TREE_ASSERT_OK(tree);
}

void Node_fill(struct Node* node,  
               struct Node* left_node, 
               int type, char* string, double value, struct Node* previous_node, 
               struct Node* right_node)
{   
    assert(node != nullptr);

    node->left  = left_node;
    if (node->left != nullptr) node->left->prev = node;

    node->type  = type;
    node->str   = string;
    node->value = value;
    node->prev  = previous_node;

    node->right = right_node;
    if (node->right != nullptr) node->right->prev = node;
}

struct Node* Node_create_and_fill(struct Node* left_node, 
                                  int type, char* string, double value, struct Node* previous_node, 
                                  struct Node* right_node)
{
    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));

    Node_fill(node,
              left_node,
              type, string, value, previous_node,
              right_node);
              
    return node;
}

void Tree_destruct(struct Tree* tree)
{
    Tree_null_check(tree);

    Subtree_destruct(tree->root);

    free(tree->name_equation);
    
    tree->root          = nullptr;
    tree->name_equation = nullptr;
    tree->size          = 0;
    tree->error         = 0;
}

void Subtree_destruct(struct Node* current_node)
{   
    assert(current_node != nullptr);

    if (current_node->left  != nullptr) Subtree_destruct(current_node->left);
    if (current_node->right != nullptr) Subtree_destruct(current_node->right);

    if (current_node->str != nullptr) free(current_node->str);

    Node_fill(current_node,
              nullptr,
              0, nullptr, NAN, nullptr,
              nullptr);
    
    free(current_node);
}

void Node_destruct(struct Node* current_node)
{
    assert(current_node != nullptr);

    if (current_node->str != nullptr) free(current_node->str);

    Node_fill(current_node,
              nullptr,
              0, nullptr, NAN, nullptr,
              nullptr);
    
    free(current_node);
}

struct Tree* Tree_clone(struct Tree* old_tree)
{
    Tree_null_check(old_tree);

    struct Tree* new_tree = (struct Tree*) calloc(1, sizeof(struct Tree));

    new_tree->name_equation = strdup(old_tree->name_equation);
    new_tree->size          = old_tree->size;
    new_tree->error         = old_tree->error;

    if (old_tree->root != nullptr) new_tree->root = Node_clone(old_tree->root, nullptr);

    return new_tree;
}

struct Node* Node_clone(struct Node* current_node_for_clone, struct Node* previous_node_clone)
{
    assert(current_node_for_clone != nullptr);

    struct Node* current_node = Node_create_and_fill(nullptr,
                                                     current_node_for_clone->type, strdup_n(current_node_for_clone->str), 
                                                     current_node_for_clone->value, previous_node_clone,
                                                     nullptr);

    if (current_node_for_clone->left  != nullptr) current_node->left  = Node_clone(current_node_for_clone->left,  current_node);
    if (current_node_for_clone->right != nullptr) current_node->right = Node_clone(current_node_for_clone->right, current_node);

    return current_node;
}

struct Stack* Tree_search(const char* name, struct Tree* tree)
{
    Tree_null_check(tree);
    assert(name != nullptr);

    struct Stack* path_element = (struct Stack*) calloc(1, sizeof(struct Stack));
    
    Stack_construct(path_element, 100);

    Node_search(name, tree->root, path_element);
    
    return path_element;
}

int Node_search(const char* name, struct Node* current_node, struct Stack* path_element)
{
    assert(name         != nullptr);
    assert(current_node != nullptr);
    assert(path_element != nullptr);

    Stack_push(path_element, current_node);

    if (strcmp(name, current_node->str) == 0) 
    {
        return 1;
    }

    else
    {
        if (current_node->left != nullptr && current_node->right != nullptr)
        {
            if (Node_search(name, current_node->left,  path_element) ||
                Node_search(name, current_node->right, path_element))
            {
                return 1;
            }

            else
            {
                Stack_pop(path_element);
                return 0;
            }
        }

        else if (current_node->left == nullptr && current_node ->right == nullptr)
        {
            Stack_pop(path_element);
            return 0;
        }

        return 0;
    }
}



void Tree_null_check(struct Tree* tree)
{
    if (tree == nullptr)
    {
        FILE* log = fopen("log.txt", "a");

        fprintf(log, "Tree (ERROR NULL PTR) [0x000000] \n");
        fflush(log);

        abort();
    }
}

int Tree_ERROR(struct Tree* tree)
{
    Tree_null_check(tree);

    if (tree->error != 0)
    {
        return tree->error;
    }


    if ((long long)tree->size < 0)
    {
        tree->error = NEGATIVE_SIZE;
        return NEGATIVE_SIZE;
    }

    if (tree->root != nullptr)
    {
        size_t count = 0;

        Node_validator(tree, tree->root, &count);

        if (tree->error != 0)
        {
            return tree->error;
        }

        if (tree->size != count)
        {
            tree->error = WRONG_SIZE;
            return WRONG_SIZE;
        }
    }

    return 0;
}

void Node_validator(struct Tree* tree, struct Node* current_node, size_t* count)
{
    ++(*count);

    if (tree->error != 0) return;

    if (fabs(current_node->type - 4) > 3)
    {
        tree->error = TREE_WRONG_NODE_TYPE;
        return;
    }

    if ((current_node->type != NUMBER) && (current_node->str == nullptr))
    {
        tree->error = TREE_WRONG_NODE_STR;
        return;
    }

    if ((current_node->type == VAR) && (current_node->right != nullptr))
    {
        tree->error = TREE_WRONG_VAR_CHILD;
        return;
    }

    if ((current_node->type == NUMBER) && (current_node->right != nullptr))
    {
        tree->error = TREE_WRONG_NUMBER_CHILD;
        return;
    }

    if (current_node->left  != nullptr) Node_validator(tree, current_node->left,  count);
    if (current_node->right != nullptr) Node_validator(tree, current_node->right, count);
}

void Tree_dump(struct Tree* tree, FILE* log)
{
    Tree_null_check(tree);
    
    const char* code = Tree_text_ERROR(tree);

    fprintf(log, "Tree (ERROR #%d: %s [%p] \"%s\" Equation: \"%s\" \n"
                  "{\n"
                  "\tsize = %lu\n" 
                  "\tdata[%p]\n",  
                  tree->error, code, tree, tree->name_tree + 1, tree->name_equation, tree->size, tree->root);

    size_t count = 0;

    if (tree->root != nullptr) Print_node(tree, tree->root, &count, log);

    fprintf(log, "\t}\n");

    fflush(log);
}

void Print_node(struct Tree* tree, struct Node* current_node, size_t* count, FILE* log)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(count        != nullptr);
    assert(log          != nullptr);

    if (*count > tree->size) return;

    if (current_node->left != nullptr) Print_node(tree, current_node->left, count, log);

    ++(*count);

    fprintf(log, "\t\tNode %lu [%p]: Type: %d; Value: %lg; Str: %s; Left: [%p]; Right: [%p];\n",
            *count, current_node, current_node->type, current_node->value, current_node->str,
            current_node->left, current_node->right);

    if (current_node->right != nullptr) Print_node(tree, current_node->right, count, log);
}

void Tree_graph(struct Tree* tree)
{
    Tree_null_check(tree);

    static size_t number_call = 0;

    ++number_call;

    FILE* text = fopen("images/tree_graph.dot", "w");

    fprintf(text, "digraph G {\n"            
                  "graph [bgcolor = Snow2]\n");

    size_t count = 0;

    Node_graph(tree, tree->root, &count, text);

    fprintf(text, "\tlabelloc=\"t\";\n"
                  "\tlabel=\"Equation: %s \";\n"
                  "}\n", tree->name_equation);

    fclose(text);

    char command[100] = {};

    sprintf(command, "dot -Tjpeg -o images/base_%lu.jpeg images/tree_graph.dot", number_call);
    system(command);

    sprintf(command, "gwenview images/base_%lu.jpeg", number_call);
    system(command);
}

void Node_graph(struct Tree* tree, struct Node* current_node, size_t* count, FILE* text)
{   
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(count        != nullptr);
    assert(text         != nullptr);

    if (*count > tree->size) return;

    ++(*count);

    if (current_node->type == NUMBER)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%lg\", style = filled, color = black, fillcolor = green]\n", current_node, current_node->value);
    }

    if (current_node->type == VAR)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\"style = filled, color = black, fillcolor = orange]\n", current_node, current_node->str);
    }

    if (current_node->type == OPERATION)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\" style = filled, color = black, fillcolor = lightskyblue]\n", current_node, current_node->str);
    }

    if (current_node->type == FUNC)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\"style = filled, color = black, fillcolor = crimson]\n", current_node, current_node->str);
    }

    if (current_node->type == DECLARATE)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\"style = filled, color = black, fillcolor = purple1]\n", current_node, current_node->str);
    }

    if (current_node->type == SEPARATOR)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\"style = filled, color = black, fillcolor = yellow]\n", current_node, current_node->str);
    }

    if (current_node->type == LR)
    {
        fprintf(text, "\t\"%p\" [shape = box, label = \"%s\"style = filled, color = black, fillcolor = azure3]\n", current_node, current_node->str);
    }

    if (current_node->left != nullptr)
    {
        fprintf(text, "\t\"%p\" -> \"%p\" \n", current_node, current_node->left);
    }

    if (current_node->right != nullptr)
    {
        fprintf(text, "\t\"%p\" -> \"%p\" \n", current_node, current_node->right);
    }

    // fprintf(text, "\t\"%p\" -> \"%p\" \n", current_node, current_node->prev);

    if (current_node->left  != nullptr) Node_graph(tree, current_node->left,  count, text);
    if (current_node->right != nullptr) Node_graph(tree, current_node->right, count, text);      
}

const char* Tree_text_ERROR(struct Tree* tree)
{   
    Tree_null_check(tree);

    switch (tree->error)
    {
        case 0: return "OK";
        
        case_of_switch(TREE_NEGATIVE_SIZE)
        case_of_switch(TREE_WRONG_SIZE)

        case_of_switch(TREE_WRONG_NODE_TYPE)
        case_of_switch(TREE_WRONG_NODE_VALUE)
        case_of_switch(TREE_WRONG_NODE_STR)
        case_of_switch(TREE_WRONG_FUNC_LEFT_CHILD)
        case_of_switch(TREE_WRONG_VAR_CHILD)
        case_of_switch(TREE_WRONG_NUMBER_CHILD)

        case_of_switch(PROGRAM_NOT_FOUND)

        case_of_switch(TREE_WRONG_FUNC)

        case_of_switch(EQUATION_SYNTAX_ERROR)
        case_of_switch(FUNC_SYNTAX_ERROR)
        case_of_switch(TYPE_SYNTAX_ERROR)
        case_of_switch(OPERATOR_SYNTAX_ERROR)
        case_of_switch(NUMBER_SYNTAX_ERROR)
        case_of_switch(REQUIRE_SYNTAX_ERROR)
        case_of_switch(PARAM_SYNTAX_ERROR)
        case_of_switch(SCAN_SYNTAX_ERROR)
        case_of_switch(DEC_SYNTAX_ERROR)
        case_of_switch(CONDITION_SYNTAX_ERROR)
        case_of_switch(DERIVATE_SYNTAX_ERROR)

        case_of_switch(NO_MAIN_DECLARATION)
        case_of_switch(VAR_NOT_FOUND)
        case_of_switch(WRONG_CONDITION)
        case_of_switch(WRONG_TYPE_ARG_CALL_FUNC)
        case_of_switch(GLOBAL_VAR_DEC_ERROR)
        case_of_switch(NO_INITIALIZATION_VAR)
        case_of_switch(DIF_FUNC_ERROR)

        default: return "Unknown ERROR";
    }
}
