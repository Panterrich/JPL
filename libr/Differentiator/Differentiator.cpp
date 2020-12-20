#include "Differentiator.h"
#include "DSL.h"

//========================================================================================

struct Node* Derivative(struct Tree* tree, struct Node* current_node, char* var, unsigned int hash_var)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(var  != nullptr);
   
    switch (TYPE)
    {
        case NUMBER: current_node->value = 0; break;
        case VAR: 
        {
            if (strcmp(current_node->str, var) == 0)
            {
                free(current_node->str);

                current_node->type = NUMBER;
                current_node->str = nullptr;
                current_node->value = 1;
            }

            else
            {   
                free(current_node->str);

                current_node->type = NUMBER;
                current_node->str = nullptr;
                current_node->value = 0;
            }

            break;
        }

        case OPERATION: 
        {
            switch (OP)
            {
                #define DEF_OP(operator, OP, number, code) case OP: code; break;
                #define DEF_FUNC(func, FUNC, hash, number, code)
                
                #include "commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            
            break;
        }
        
        case FUNC:
        {
            switch (OP)
            {
                #define DEF_OP(operator, OP, number, code)
                #define DEF_FUNC(func, FUNC, hash, number, code) case FUNC: code; break;
                
                #include "commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }

            break;        
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    tree->size = Size_subtree(tree, tree->root);
    
    return current_node;
}

double Evaluation(struct Tree* tree, struct Node* current_node)
{   
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(current_node != nullptr);

    switch (TYPE)
    {
        case NUMBER: return current_node->value;
        case VAR:    return current_node->value;
        case OPERATION:
            switch (OP)
            {
                case ADD:  return LCALC +  RCALC;
                case SUB:  return LCALC -  RCALC;
                case MUL:  return LCALC *  RCALC;
                case DIV:  return LCALC /  RCALC;     
                case POW:  return pow(LCALC, RCALC);
                case BAA:  return LCALC >  RCALC;
                case BAB:  return LCALC <  RCALC;
                case BAE:  return LCALC == RCALC;
                case BAAE: return LCALC >= RCALC;
                case BABE: return LCALC <= RCALC;
                case BANE: return LCALC != RCALC;

                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            break;

        case FUNC:
            switch (OP)
            {
                case LN:     return log(RCALC);
                case SIN:    return sin(RCALC);
                case COS:    return cos(RCALC);
                case TG:     return tan(RCALC);
                case CTG:    return 1 / tan(RCALC);
                case ARCSIN: return asin(RCALC);
                case ARCCOS: return acos(RCALC);
                case ARCTG:  return atan(RCALC);
                case ARCCTG: return M_PI_2 - atan(RCALC);
                case SH:     return sinh(RCALC);
                case CH:     return cosh(RCALC);
                case TH:     return tanh(RCALC);
                case CTH:    return 1 / tan(RCALC);
                case SQRT:   return sqrt(RCALC);
                case EXP:    return exp(RCALC);

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            break;

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    return 0;
}

void Optimization(struct Tree* tree)
{
    size_t old_size = 0;
    
    while (tree->size != old_size)
    {   
        old_size = tree->size;

        Constant_folding(tree, tree->root);
        Neutral_delete(tree, tree->root);
        Pow_folding(tree, tree->root);
    }
}

void Constant_folding(struct Tree* tree, struct Node* current_node)
{
    if (!Subtree_is_number(tree, current_node))
    {
        if (LNODE != nullptr) Constant_folding(tree, LNODE);
        if (RNODE != nullptr) Constant_folding(tree, RNODE);
    }
    
    else if (Subtree_is_number(tree, current_node))
    {
        double value = Evaluation(tree, current_node);

        struct Node* equal = Node_create_and_fill(nullptr,
                                                  NUMBER, nullptr, value, PREV,
                                                  nullptr);
        
        if (current_node == tree->root) 
        {
            tree->root = equal;
        }

        else
        {
            if (PREV->left  == current_node) PREV->left = equal;
            if (PREV->right == current_node) PREV->right = equal;
        }

        tree->size = Size_subtree(tree, tree->root);

        Subtree_destruct(current_node);
    }
}

void Pow_folding(struct Tree* tree, struct Node* current_node)
{
    if ((TYPE == OPERATION) && (OP == POW) && (LNODE != nullptr))
    {
        if ((LNODE->type == OPERATION) && ((char)LNODE->value == POW))
        {
            struct Node* base      = LNODE->left;
            struct Node* left_deg  = LNODE->right;
            struct Node* right_deg = RNODE;
            struct Node* mul       = LNODE;

            free(mul->str);

            Node_fill(mul, 
                      left_deg,
                      OPERATION, strdup("*"), MUL, current_node,
                      right_deg);

            Node_fill(current_node,
                      base, 
                      OPERATION, current_node->str, POW, PREV,
                      mul);
        }
    }

    else
    {
        if (LNODE != nullptr) Pow_folding(tree, LNODE);
        if (RNODE != nullptr) Pow_folding(tree, RNODE);
    }
}

void Neutral_delete(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);

    if (current_node != nullptr)
    {
        if (LNODE != nullptr) Neutral_delete(tree, LNODE);
        if (RNODE != nullptr) Neutral_delete(tree, RNODE);
    }
    else
    {
        return;
    }

    if (TYPE == OPERATION)
    {
        switch (OP)
        {
            case ADD:
            case SUB:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }
                
                break;
            }

            case MUL:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }
                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }

                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }
                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                break;
            }
            
            case DIV:
            {   
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                break;
            }

            case POW:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && ((Is_equal(LNODE->value, 0)) || (Is_equal(LNODE->value, 1))))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && ((Is_equal(RNODE->value, 0)) || (Is_equal(RNODE->value, 1))))
                    {
                        struct Node* result = RNODE;
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        
                        result->value = 1;

                        return;
                    }
                }

                break;
            }

            default:
                break;
        }
    }

    else if (TYPE == FUNC)
    {   
        switch (OP)
        {
            case LN:
            case ARCCOS:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(LNODE->value, 1)))
                {
                    Replace_func(tree, current_node, 0);
                    return;

                    break;
                }
            }

            case SIN:
            case TG:
            case ARCSIN:
            case ARCTG:
            case SH:
            case TH:
            case SQRT:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(LNODE->value, 0)))
                {
                    Replace_func(tree, current_node, 0);
                    return;
                }

                break;
            }
            
            case COS:
            case CH:
            case EXP:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(LNODE->value, 0)))
                {
                    Replace_func(tree, current_node, 1);
                    return;
                }

                break;
            }

            default: break;
        }
    }
}

void Replace_func(struct Tree* tree, struct Node* current_node, double value)
{
    Subtree_destruct(LNODE);
    free(current_node->str);

    Node_fill(current_node,
              nullptr,
              NUMBER, nullptr, value, current_node->prev,
              nullptr);

    tree->size = Size_subtree(tree, tree->root);
}

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete)
{
    Tree_null_check(tree);

    if (current_node == tree->root) 
    {
        tree->root = subtree_linking;
        subtree_linking->prev = nullptr;
    }

    else
    {
        subtree_linking->prev = PREV;

        if (PREV->left  == current_node) PREV->left  = subtree_linking;
        if (PREV->right == current_node) PREV->right = subtree_linking;
    }
    
    tree->size = Size_subtree(tree, tree->root);

    Subtree_destruct(subtree_delete);
    Node_destruct(current_node);
}

//=========================================================================================

char* Name_op(int number_op)
{
    #define DEF_OP(operator, OP, number, code) if (number_op == number) return strdup(#operator); 
    #define DEF_FUNC(func, FUNC, hash, number, code)  
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC

    return nullptr; 
}

unsigned int Hash(const char* cmd)
{
    assert(cmd != nullptr);

    unsigned int hash = 0;
    
    for (size_t i = 0; i < strlen(cmd); ++i)
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

size_t Skip_spaces(char** string)
{   
    assert(string != nullptr);

    size_t count = 0;

    while (isspace(**string))
    {
        ++(*string);
        ++count;
    }

    return count; 
}

int Code_func(char* func)
{
    assert(func != nullptr);

    unsigned int hash_func = Hash(func);

    #define DEF_OP(operator, OP, number, code) 
    #define DEF_FUNC(func, FUNC, hash, number, code) if (hash_func == hash) return FUNC;  
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC

    return -1;
}

int Subtree_is_number(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);

    if (current_node == nullptr) return 0;

    switch (TYPE)
    {
        case VAR:
        case FUNC:
        case DECLARATE:
        case LR:
            return 0;
        
        case NUMBER:
            return 1;
        
        case OPERATION:
            return Subtree_is_number(tree, LNODE) * 
                   Subtree_is_number(tree, RNODE);

        default:
            printf("ERROR: Unknow type: %d on line %d\n", TYPE, __LINE__);
            return 0;
            break;
    }
    
    return 0;   
}

size_t Size_subtree(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);

    size_t count = 0;

    Node_count(tree, current_node, &count);

    return count;
}

void Node_count(struct Tree* tree, struct Node* current_node, size_t* count)
{   
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(count        != nullptr);

    ++(*count);

    if (LNODE != nullptr) Node_count(tree, LNODE, count);
    if (RNODE != nullptr) Node_count(tree, RNODE, count);
}

int Is_equal(double value, double number)
{
    return fabs(value - number) <= ACCURACY;
}
