#include "Onegin.h"

//==============================================================================================

void Create_text(FILE* file, struct Text* input_text)
{
    assert(file != NULL);
    assert(input_text != NULL);

    input_text->buffer = Create_buffer(file, &(input_text->n_lines), &(input_text->size));
    input_text->lines = Placing_pointers_in_text(input_text->buffer, input_text->n_lines);
}

size_t Size_file(FILE* file)
{
    assert(file != NULL);

    struct stat info = {};
    fstat(fileno(file), &info);

    return (size_t)info.st_size;
}

char* Create_buffer(FILE* file, size_t* n_lines, size_t* size)
{
    assert(file != NULL);
    assert(n_lines != NULL);
    assert(size != NULL);

    *size = Size_file(file);

    char* buffer = (char*) calloc(*size, sizeof(char));

    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(char), *size, file);

    char* begin_line = buffer;
    char* end_line = buffer;

    size_t count = 0;

    while ((end_line = strchr(begin_line, '\n')) != NULL)
    {
        begin_line = end_line + 1;
        count++;
    }

    *n_lines = count;

    return buffer;
}

struct String* Placing_pointers_in_text(char* buffer, const size_t n_lines) 
{
    assert(buffer != NULL);
    assert(n_lines != 0);

    struct String* strings = (struct String*) calloc(n_lines, sizeof(struct String));

    assert(strings != NULL);

    char* pointer_begin = buffer;
    char* pointer_end = buffer;
    char* pointer_comment = buffer;

    for (int i_line = 0; i_line < n_lines ; i_line++) 
    {
        pointer_end = strchr(pointer_begin, '\n');
        *pointer_end = '\0';

        pointer_comment = strchr(pointer_begin, ';');

        if (pointer_comment != NULL)
        {
            *pointer_comment = '\0';
            (strings + i_line)->len = pointer_comment - pointer_begin;
        }

        else
        {
            (strings + i_line)->len = pointer_end - pointer_begin;
        }
        
        (strings + i_line)->str = pointer_begin;
        
        pointer_begin = pointer_end + 1;
    }

    return strings;
}

void Print_buffer(FILE* file, const struct Text* input_text)
{
    assert(file != NULL);
    assert(input_text != NULL);

    char* pointer_begin = input_text->buffer;

    for (int index_strings = 0; index_strings < input_text->n_lines; ++index_strings)
    {
        fputs(pointer_begin, file);
        fputc('\n', file);

        pointer_begin = strchr(pointer_begin, '\0') + 1;
    }
}

void Print_text(FILE* file, const struct Text* input_text)
{
    assert(file != NULL);
    assert(input_text != NULL);

    for (int index_strings = 0; index_strings < input_text->n_lines; index_strings++)
    {   
        fputs((input_text->lines)[index_strings].str, file);
        fputc('\n', file);
    }
}

void Free_memory(struct Text* text)
{
    free(text->lines);
    free(text->buffer);
    
    text->lines = NULL;
    text->buffer = NULL;
}