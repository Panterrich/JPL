#include "Backend.h"

int main(int argc, char* argv[])
{
       const char* name_program = nullptr;

    if (argc < 2)
    {
        printf("Напишите имя дерева\n");
        return 1;
    }

    if (argc == 2)
    {
        name_program = argv[1];
    }

    FILE* file = fopen(name_program, "rb");

    if (file == nullptr)
    {
        printf("Введите корректное имя дерева\n");
        return 1;
    }

    struct Text text = {};
    Create_text(file, &text);

    fclose(file);

    struct Tree tree = {};
    TREE_CONSTRUCT(&tree);

    Tree_create(&tree, &text, name_program);
    Free_memory(&text);
    Tree_graph(&tree);

    Generate_ASM_code(&tree);
    
    Tree_destruct(&tree);
    return 0;
}