#include "Frontend.h"
#include <locale.h>

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ja_JP.utf8");

    const char* name_program = nullptr;

    if (argc < 2)
    {
        printf("Напишите имя программы\n");
        return 1;
    }

    if (argc == 2)
    {
        name_program = argv[1];
    }

    FILE* file = fopen(name_program, "rb");

    if (file == nullptr)
    {
        printf("Введите корректное имя файла!\n");
        return 1;
    }

    struct Program program = {};
    Program_create(file, &program);
    
    fclose(file);

    struct Stack nodes = {};
    STACK_CONSTRUCT(&nodes, INITIAL_CAPACITY);

    Tokenizer(&program, &nodes);

    struct Tree tree = {};
    TREE_CONSTRUCT(&tree);

    GetProg(&tree, &nodes, name_program);

    Free_nodes(&nodes);

    Tree_graph(&tree);
    Tree_print(&tree);
    
    Tree_destruct(&tree);

    return 0;
}