#include "Frontend.h"
//==========================================================================

void Program_create(FILE* file, struct Program* program)
{   
    size_t size = Size_file(file);

    program->buffer = (char*) calloc(size, sizeof(char));
    program->current_symbol = program->buffer;

    fseek(file, 0, SEEK_SET);
    fread(program->buffer, sizeof(char), size, file);
}

void Program_destruct(struct Program* program)
{
    free(program->buffer);

    program->buffer         = nullptr;
    program->current_symbol = nullptr;
}

void Tokenizer(struct Program* program, struct Stack* nodes)
{
    struct Node* current_node = nullptr;
    size_t count = 0;

    while(*program->current_symbol != '\0')
    {
        Skip_separator(&program->current_symbol);

        #define KEY_WORD(word, count, replace, type, value, number, path) if (strncmp(program->current_symbol, word, count) == 0)                         \
                                                                          {                                                                               \
                                                                              current_node = Node_create_and_fill(nullptr,                                \
                                                                                                                  type, strdup(replace), value, nullptr,  \
                                                                                                                  nullptr);                               \
                                                                              Stack_push(nodes, current_node);                                            \
                                                                              program->current_symbol += count;                                           \
                                                                              continue;                                                                   \
                                                                          }
        #ifdef HG
            #include "../libr/key_words_hg.h"
        #else
            #include "../libr/key_words_rm.h"
        #endif
        
        #undef KEY_WORD

        if (isdigit((int)(unsigned char)*program->current_symbol) || (((int)(unsigned char)*program->current_symbol == '-') && (isdigit((int)(unsigned char)*(program->current_symbol + 1)))))
        {
            double value = strtod(program->current_symbol, &program->current_symbol);

            current_node = Node_create_and_fill(nullptr,
                                                NUMBER, nullptr, value, nullptr,
                                                nullptr);
            Stack_push(nodes, current_node);
            continue;
        }

        if (isalpha((int)(unsigned char)*program->current_symbol))
        {
            char* begin = program->current_symbol;

            while((isalpha((int)(unsigned char)*program->current_symbol)  || 
                   isdigit((int)(unsigned char)*program->current_symbol)  ||
                   (*program->current_symbol == '_')) && (*program->current_symbol != '\0'))
            {
                ++(program->current_symbol); 
            }

            current_node = Node_create_and_fill(nullptr,
                                                VAR, strndup(begin, program->current_symbol - begin), NAN, nullptr,
                                                nullptr);
            Stack_push(nodes, current_node);
            
            continue;
        }

        Skip_separator(&program->current_symbol);

        ++count;
        if (count > 100000) 
        {
            printf("ERROR: %s \n", program->current_symbol);
            break;
        }
    } 
}

size_t Skip_separator(char** string)
{   
    assert(string != nullptr);

    size_t count = 0;

    while (isspace((int)(unsigned char)**string) || **string == '\n')
    {
        ++(*string);
        ++count;
    }

    return count; 
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

struct Node* Get_node(struct Stack* nodes, size_t index) 
{
    return ((struct Node*)((nodes->data)[index]));
}

struct Node* GetProg(struct Tree* tree, struct Stack* nodes, const char* name_program)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(name_program != nullptr);

    tree->name_equation = Get_name_file(name_program);

    char path[MAX_SIZE_COMMAND] = "";
    sprintf(path, "files/%s.list", tree->name_equation);

    FILE* list = fopen(path, "w");

    size_t index_node = 0;

    tree->root = GetDec(tree, nullptr, nodes, &index_node, list);
    fprintf(list, "file '../sound/end.ts'\n");

    TREE_ASSERT_OK(tree);
    fclose(list);

    // char command[MAX_SIZE_COMMAND] = "";
    // sprintf(command, "ffmpeg -f concat -safe 0 -i %s -vcodec copy -acodec copy files/%s.mp4", path, tree->name_equation);
    // system(command);

    return tree->root;
}

struct Node* GetDec(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    if (*index_node == nodes->size) return nullptr;

    if ((Get_node(nodes, *index_node)->type == FUNC) && (Get_node(nodes, *index_node)->value == 1)) 
    {
        fprintf(list, Get_path(KEY_MAIN));

        struct Node* new_func = GetFunc(tree, nullptr, nodes, index_node, list);

        struct Node* new_dec = GetDec(tree, nullptr, nodes, index_node, list);

        struct Node* dec = Node_create_and_fill(new_func, 
                                                DECLARATE, strdup("DEC"), NAN, previous_node,
                                                new_dec);
        ++(tree->size);
        
        return dec;
    }
    
    else if (Get_node(nodes, *index_node)->type == VAR)
    {
        if (*index_node + 1 <= nodes->size)
        {
            if ((Get_node(nodes, *index_node + 1)->type  == SEPARATOR) && 
                (Get_node(nodes, *index_node + 1)->value == 22))
            {
                struct Node* new_func = GetFunc(tree, nullptr, nodes, index_node, list);

                struct Node* new_dec = GetDec(tree, nullptr, nodes, index_node, list);

                struct Node* dec = Node_create_and_fill(new_func, 
                                                        DECLARATE, strdup("DEC"), NAN, previous_node,
                                                        new_dec);
                ++(tree->size);
                
                return dec;
            }

            else 
            {
                struct Node* var = GetGlobal(tree, nullptr, nodes, index_node, list);

                struct Node* new_dec = GetDec(tree, nullptr, nodes, index_node, list);

                struct Node* dec = Node_create_and_fill(var, 
                                                        DECLARATE, strdup("DEC"), NAN, previous_node,
                                                        new_dec);
                ++(tree->size);
                
                return dec;
            }
        }

        else
        {
            tree->error = DEC_SYNTAX_ERROR;

            Syntax_error(nodes, *index_node);
            TREE_ASSERT_OK(tree);
        }       
    }

    return nullptr;
}

struct Node* GetGlobal(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);

    struct Node* var = Get_node(nodes, *index_node);
    ++(*index_node);
    ++(tree->size);

    if (Get_node(nodes, *index_node)->type == LR        && Get_node(nodes, *index_node)->value == 3) *(int*)0 = 0;
    if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 32)
    {
        struct Node* op = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        if (Get_node(nodes, *index_node)->type == NUMBER)
        {
            struct Node* num = Get_node(nodes, *index_node);
            ++(*index_node);
            ++(tree->size);

            Node_fill(op, 
                      var, 
                      OPERATION, op->str, 32, previous_node,
                      num);
            
            Require(tree, nodes, index_node, LR, 3);
            fprintf(list, Get_path(KEY_LR));

            return op;
        }

        else
        {   
            tree->error = GLOBAL_VAR_DEC_ERROR;
            TREE_ASSERT_OK(tree);
        }
    }

    else
    {
        tree->error = GLOBAL_VAR_DEC_ERROR;
        TREE_ASSERT_OK(tree);
    }

    return nullptr;
}

struct Node* GetFunc(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* name_func = Get_node(nodes, *index_node);
    ++(*index_node);
    ++(tree->size);

    Require(tree, nodes, index_node, SEPARATOR, 22);
    struct Node* params = GetParam(tree, nullptr, nodes, index_node);
    Require(tree, nodes, index_node, SEPARATOR, 23);

    Require(tree, nodes, index_node, SEPARATOR, 20);
    fprintf(list, Get_path(KEY_LSB));

    struct Node* body = GetBody(tree, nullptr, nodes, index_node, list);
    Require(tree, nodes, index_node, SEPARATOR, 21);
    fprintf(list, Get_path(KEY_RSB));

    Node_fill(name_func, 
              params, 
              FUNC, name_func->str, NAN, previous_node, 
              body);

    return name_func;
}

struct Node* GetParam(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);

    if (Get_node(nodes, *index_node)->type != SEPARATOR || Get_node(nodes, *index_node)->value != 23)
    {
        struct Node* param = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        if (param->type != VAR)
        {
            tree->error = PARAM_SYNTAX_ERROR;

            Syntax_error(nodes, *index_node);
            TREE_ASSERT_OK(tree);
        }

        if (Get_node(nodes, *index_node)->type == SEPARATOR && Get_node(nodes, *index_node)->value == 24)
        {
            Require(tree, nodes, index_node, SEPARATOR, 24);

            struct Node* new_param = GetParam(tree, param, nodes, index_node);

            Node_fill(param,
                      new_param,
                      VAR, param->str, NAN, nullptr,
                      nullptr);
        }

        return param;
    }

    return nullptr;
}

struct Node* GetBody(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    if (Get_node(nodes, *index_node)->type != SEPARATOR || Get_node(nodes, *index_node)->value != 21)
    {
        struct Node* op     = GetOp(tree, nullptr, nodes, index_node, list);
        struct Node* new_LR = GetBody(tree, nullptr, nodes, index_node, list);

        struct Node* current_LR = Node_create_and_fill(op,
                                                       LR, strdup("LR"), NAN, nullptr,
                                                       new_LR);
        ++(tree->size);

        return current_LR;
    }

    else 
    {
        return nullptr;
    }   
}

struct Node* GetOp(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    if (Get_node(nodes, *index_node)->type == LR && Get_node(nodes, *index_node)->value == 3)
    {
        ++(*index_node);
        return nullptr;
    }

    if (Get_node(nodes, *index_node)->type == FUNC && Get_node(nodes, *index_node)->value == 10)
    {
        return GetPrint(tree, nullptr, nodes, index_node, list);
    }

    if (Get_node(nodes, *index_node)->type == FUNC && Get_node(nodes, *index_node)->value == 11)
    {
        return GetScan(tree, nullptr, nodes, index_node, list);
    }

    if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 30)
    {
        return GetIf(tree, nullptr, nodes, index_node, list);
    }

    if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 31)
    {
        return GetWhile(tree, nullptr, nodes, index_node, list);
    }

    if (Get_node(nodes, *index_node)->type == VAR && Get_node(nodes, *index_node + 1)->type  == OPERATION && 
                                                     Get_node(nodes, *index_node + 1)->value == 32)
    {
        return GetAssign(tree, nullptr, nodes, index_node, list);
    }

    // if ((Get_node(nodes, *index_node)->type == VAR                                                      || 
    //    ((Get_node(nodes, *index_node)->type == FUNC)                                                    && 
    //     (Get_node(nodes, *index_node)->value < 65 || Get_node(nodes, *index_node)->value > 79)))        &&
    //     (Get_node(nodes, *index_node + 1)->type == SEPARATOR && Get_node(nodes, *index_node + 1)->value == 22))
    // {
    //     return GetCall(tree, nullptr, nodes, index_node, list);
    // }

    else
    {   
        return GetRet(tree, nullptr, nodes, index_node, list);
    }
}   

struct Node* GetPrint(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* print = Get_node(nodes, *index_node);
    ++(tree->size);
    ++(*index_node);

    fprintf(list, Get_path(KEY_PRINT));

    struct Node* equation = GetC(tree, nullptr, nodes, index_node, list);

    Node_fill(print,
              equation,
              FUNC, print->str, 11, nullptr,
              nullptr);

    Require(tree, nodes, index_node, LR, 3);
    fprintf(list, Get_path(KEY_LR));

    return print;
}

struct Node* GetScan(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* scan = Get_node(nodes, *index_node);
    ++(tree->size);
    ++(*index_node);

    fprintf(list, Get_path(KEY_SCAN));

    if (Get_node(nodes, *index_node)->type != VAR)
    {
        tree->error = SCAN_SYNTAX_ERROR;

        Syntax_error(nodes, *index_node);
        TREE_ASSERT_OK(tree);
    }

    struct Node* var = Get_node(nodes, *index_node);
    ++(tree->size);
    ++(*index_node);

    Node_fill(scan,
              var, 
              FUNC, scan->str, 11, nullptr,
              nullptr);

    Require(tree, nodes, index_node, LR, 3);
    fprintf(list, Get_path(KEY_LR));

    return scan;
}

struct Node* GetIf(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* op = Get_node(nodes, *index_node);
    ++(tree->size);
    ++(*index_node);

    fprintf(list, Get_path(KEY_IF));

    Require(tree, nodes, index_node, SEPARATOR, 22);
    struct Node* condition = GetC(tree, nullptr, nodes, index_node, list);
    Require(tree, nodes, index_node, SEPARATOR, 23);


    struct Node* actions = GetGroup(tree, nullptr, nodes, index_node, list);    
    
    Node_fill(op,
              condition, 
              OPERATION, op->str, 30, nullptr,
              actions);
    
    return op;
}

struct Node* GetWhile(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);


    struct Node* op = Get_node(nodes, *index_node);
    ++(tree->size);
    ++(*index_node);

    fprintf(list, Get_path(KEY_WHILE));

    Require(tree, nodes, index_node, SEPARATOR, 22);
    struct Node* condition = GetC(tree, nullptr, nodes, index_node, list);
    Require(tree, nodes, index_node, SEPARATOR, 23);

    struct Node* actions = GetGroup(tree, nullptr, nodes, index_node, list);
    
    Node_fill(op,
              condition, 
              OPERATION, op->str, 31, nullptr,
              actions);
    
    return op;
}

struct Node* GetAssign(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* var = Get_node(nodes, *index_node);
    ++(*index_node);
    ++(tree->size);

    struct Node* op = Get_node(nodes, *index_node);
    ++(*index_node);
    ++(tree->size);

    fprintf(list, Get_path(KEY_ASSIGN));

    struct Node* equataion = GetC(tree, nullptr, nodes, index_node, list);

    Node_fill(op,
              var, 
              OPERATION, op->str, 32, nullptr,
              equataion);
    
    Require(tree, nodes, index_node, LR, 3);
    fprintf(list, Get_path(KEY_LR));

    return op;

}

// struct Node* GetCall(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
// {
//     Tree_null_check(tree);
//     Stack_null_check(nodes);
//     assert(index_node != nullptr);
//     assert(list       != nullptr);

//     struct Node* func = Get_node(nodes, *index_node);
//     ++(tree->size);
//     ++(*index_node);

//     Require(tree, nodes, index_node, SEPARATOR, 22);
//     struct Node* param = GetArg(tree, nullptr, nodes, index_node, list);
//     Require(tree, nodes, index_node, SEPARATOR, 23);
//     Require(tree, nodes, index_node, LR, 3);
//     fprintf(list, Get_path(KEY_LR));

//     Node_fill(func, 
//               param, 
//               FUNC, func->str, NAN, previous_node, 
//               nullptr);
              
//     return func;
// }

struct Node* GetRet(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* equation = GetC(tree, nullptr, nodes, index_node, list);

    // if (Get_node(nodes, *index_node)->type != OPERATION || Get_node(nodes, *index_node)->value != 2)
    // {
    //     tree->error = RETURN_SYNTAX_ERROR;

    //     Syntax_error(nodes, *index_node);
    //     TREE_ASSERT_OK(tree);
    // }

    if (Get_node(nodes, *index_node)->type == LR && Get_node(nodes, *index_node)->value == 3)
    {
        Require(tree, nodes, index_node, LR, 3);
        fprintf(list, Get_path(KEY_LR));    
        
        return equation;
    }

    else if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 2)
    {
        struct Node* ret = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        fprintf(list, Get_path(KEY_RETURN));

        Node_fill(ret,
                equation,
                OPERATION, ret->str, 2, nullptr,
                nullptr);
    
        Require(tree, nodes, index_node, LR, 3);
        fprintf(list, Get_path(KEY_LR));    
        
        return ret;
    } 

    else
    {
        tree->error = EQUATION_SYNTAX_ERROR;

        Syntax_error(nodes, *index_node);
        TREE_ASSERT_OK(tree);
    }

    return equation;
}

struct Node* GetC(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* left = GetE(tree, nullptr, nodes, index_node, list);

    if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value >= 46 &&
                                                           Get_node(nodes, *index_node)->value <= 51)
    {
        struct Node* op = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        struct Node* right = GetE(tree, nullptr, nodes, index_node, list);

        Node_fill(op, 
                left, 
                OPERATION, op->str, op->value, nullptr,
                right);

        return op;
    }

    return left;
}

struct Node* GetE(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* current_node = GetT(tree, nullptr, nodes, index_node, list);

    while (Get_node(nodes, *index_node)->type == OPERATION && (Get_node(nodes, *index_node)->value == 41 || 
                                                               Get_node(nodes, *index_node)->value == 42))
    {
        struct Node* op = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        fprintf(list, Get_path(op->value));

        struct Node* left  = current_node;
        struct Node* right = GetT(tree, nullptr, nodes, index_node, list);

        Node_fill(op,
                  left, 
                  OPERATION, op->str, op->value, previous_node,
                  right);
                        
        current_node = op;
    }

    return current_node;
}

struct Node* GetT(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* current_node = GetD(tree, previous_node, nodes, index_node, list);

    while (Get_node(nodes, *index_node)->type == OPERATION && (Get_node(nodes, *index_node)->value == 43 || 
                                                               Get_node(nodes, *index_node)->value == 44))
    {
        struct Node* op = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        fprintf(list, Get_path(op->value));

        struct Node* left  = current_node;
        struct Node* right = GetD(tree, nullptr, nodes, index_node, list);

        Node_fill(op,
                  left, 
                  OPERATION, op->str, op->value, previous_node,
                  right);
                        
        current_node = op;
    }

    return current_node;
}

struct Node* GetD(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* current_node = GetP(tree, nullptr, nodes, index_node, list);

    while (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 45)
    {
        struct Node* op = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        fprintf(list, Get_path(op->value));

        struct Node* left  = current_node;
        struct Node* right = GetD(tree, nullptr, nodes, index_node, list);

        Node_fill(op,
                  left, 
                  OPERATION, op->str, op->value, previous_node,
                  right);
                        
        current_node = op;
    }

    return current_node;
}

struct Node* GetP(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    if (Get_node(nodes, *index_node)->type == SEPARATOR && Get_node(nodes, *index_node)->value == 22)
    {
        ++(*index_node);

        struct Node* current_node = GetE(tree, previous_node, nodes, index_node, list);

        Require(tree, nodes, index_node, SEPARATOR, 23);

        if (Get_node(nodes, *index_node)->type == OPERATION && Get_node(nodes, *index_node)->value == 52)
        {
            struct Node* der = Get_node(nodes, *index_node);
            ++(*index_node);
            ++(tree->size);

            fprintf(list, Get_path(KEY_DIF));

            if (Get_node(nodes, *index_node)->type != VAR)
            {
                tree->error = DERIVATE_SYNTAX_ERROR;

                Syntax_error(nodes, *index_node);
                TREE_ASSERT_OK(tree);
            }

            struct Node* var = Get_node(nodes, *index_node);
            ++(*index_node);
            ++(tree->size);

            Node_fill(der, 
                      var,
                      OPERATION, der->str, der->value, nullptr,
                      current_node);

            return der;
        }
        
        return current_node;
    }

    else return GetW(tree, previous_node, nodes, index_node, list);
}

struct Node* GetN(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    struct Node* number = Get_node(nodes, *index_node);

    if (number->type != NUMBER)
    {
        tree->error = NUMBER_SYNTAX_ERROR;

        Syntax_error(nodes, *index_node);
        TREE_ASSERT_OK(tree);
    }

    ++(*index_node);
    ++(tree->size);

    return number;
}

struct Node* GetW(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);

    if (Get_node(nodes, *index_node)->type == NUMBER) return GetN(tree, nullptr, nodes, index_node, list);
    if ((Get_node(nodes, *index_node)->type == FUNC) || (Get_node(nodes, *index_node)->type == VAR && 
         Get_node(nodes, *index_node + 1)->type == SEPARATOR && 
         Get_node(nodes, *index_node + 1)->value == 22))
    {
        struct Node* func = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        struct Node* arg = nullptr;
        Require(tree, nodes, index_node, SEPARATOR, 22);

        if (func->type == FUNC && func->value >= 65 && func->value <= 79)
        { 
            arg = GetE(tree, nullptr, nodes, index_node, list);   
        }

        else
        {
            arg = GetArg(tree, nullptr, nodes, index_node, list);
        }

        Require(tree, nodes, index_node, SEPARATOR, 23);
        
        Node_fill(func,
                  arg, 
                  FUNC, func->str, NAN, previous_node, 
                  nullptr);
        
        return func;
    }

    if (Get_node(nodes, *index_node)->type == VAR)
    {
        struct Node* var = Get_node(nodes, *index_node);
        ++(*index_node);
        ++(tree->size);

        return var;
    }

    return nullptr;
}

struct Node* GetGroup(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);


    if (Get_node(nodes, *index_node)->type == SEPARATOR && Get_node(nodes, *index_node)->value == 20)
    {    
        Require(tree, nodes, index_node, SEPARATOR, 20);
        fprintf(list, Get_path(KEY_LSB));
           
        struct Node* op = GetBody(tree, nullptr, nodes, index_node, list);
    
        Require(tree, nodes, index_node, SEPARATOR, 21);
        fprintf(list, Get_path(KEY_RSB));

        return op;
    }

    else return GetOp(tree, nullptr, nodes, index_node, list);
}

struct Node* GetArg(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list)
{
    Tree_null_check(tree);
    Stack_null_check(nodes);
    assert(index_node != nullptr);
    assert(list       != nullptr);


    if (Get_node(nodes, *index_node)->type != SEPARATOR || Get_node(nodes, *index_node)->value != 23)
    {
        struct Node* arg = GetC(tree, nullptr, nodes, index_node, list);
        
        if (Get_node(nodes, *index_node)->type == SEPARATOR && Get_node(nodes, *index_node)->value == 24)
        {
            Require(tree, nodes, index_node, SEPARATOR, 24);

            struct Node* new_arg = GetArg(tree, arg, nodes, index_node, list);

            struct Node* lr = Node_create_and_fill(new_arg,
                                                   LR, strdup("LR"), 3, nullptr,
                                                   arg);

            ++(tree->size);

            return lr;
        }

        return arg;
    }

    return nullptr;
}

void Require(struct Tree* tree, struct Stack* nodes, size_t* index_node, int type, double value)
{
    if (Get_node(nodes, *index_node)->type == type && Get_node(nodes, *index_node)->value == value) 
    {
        ++(*index_node);
    }

    else 
    {
        tree->error = REQUIRE_SYNTAX_ERROR;

        Syntax_error(nodes, *index_node);
        TREE_ASSERT_OK(tree);
    }
}

void Syntax_error(struct Stack* nodes, size_t index_node)
{
    FILE* log = fopen("log.txt", "a");

    for (size_t index = 0; index < index_node; ++index)
    {
        fprintf(log, "#%d TYPE: %d STR: \"%s\" VALUE: %lg \n", index, Get_node(nodes, index)->type, 
                                                                      Get_node(nodes, index)->str, 
                                                                      Get_node(nodes, index)->value);
    }

    fprintf(log, "\nОШИБКА НАЧИНАЕТСЯ ЗДЕСЬ!!!!!\n\n");

    for (size_t index = index_node; index < nodes->size; ++index)
    {
        fprintf(log, "#%d TYPE: %d STR: \"%s\" VALUE: %lg \n", index, Get_node(nodes, index)->type, 
                                                                      Get_node(nodes, index)->str, 
                                                                      Get_node(nodes, index)->value);
    }

    fprintf(log, "\n\n\n\n");
    fclose(log);
}

void Free_nodes(struct Stack* nodes)
{
    for (size_t index_node = 0; index_node < nodes->size; ++index_node)
    {
        struct Node* current_node = Get_node(nodes, index_node);

        if (current_node->type == SEPARATOR || current_node->type == LR)
        {
            free(current_node);
        }
    }

    Stack_destruct(nodes);
}

void Tree_print(struct Tree* tree)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    char name_output[MAX_SIZE_COMMAND] = {};
    sprintf(name_output, "files/%s.me", tree->name_equation);

    FILE* file = fopen(name_output, "w");
    
    Node_print(tree->root, file);

    fclose(file);

    char command[MAX_SIZE_COMMAND] = {};
    sprintf(command, "astyle --mode=cs --suffix=none --style=ansi %s", name_output);
    system(command);
}

void Node_print(struct Node* current_node, FILE* file)
{
    assert(file != nullptr);

    if (current_node == nullptr) 
    {
        fprintf(file, "{\nnil\n}\n");
        return;
    }

    if (current_node->type == FUNC)
    {    
        fprintf(file, "{\n$%s\n", current_node->str);
    }

    else if (current_node->type == NUMBER)
    {
        fprintf(file, "{\n%lg\n", current_node->value);
    }

    else
    {
        fprintf(file, "{\n%s\n", current_node->str);
    }

    Node_print(current_node->left, file);
    Node_print(current_node->right,  file);

    fprintf(file, "}\n");
}

const char* Get_path(int code)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (KEY##_##number == code) return path; 

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return nullptr;
}