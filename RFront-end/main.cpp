#include "RFrontend.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ja_JP.utf8");

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

    Tree_graph(&tree);
    Tree_print(&tree);    

    Tree_destruct(&tree);
    return 0;
}