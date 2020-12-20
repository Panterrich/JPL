#include "Assembler.h"

//======================================================

int main(int argc, const char* argv[])
{
    FILE* input = fopen(argv[1], "rb");

    if (input == NULL)
    {
        printf("File is not found, ERROR\n");
        fflush(stdout);
        return 1;
    }

    struct Text input_text = {};
    Create_text(input, &input_text);

    fclose(input);

    struct Label* labels = (struct Label*) calloc(input_text.n_lines, sizeof(struct Label));


    if (Compile_pass(&input_text, labels)) return 1;
    if (Compile_pass(&input_text, labels)) return 1;

    free(labels);
    Free_memory(&input_text);
}