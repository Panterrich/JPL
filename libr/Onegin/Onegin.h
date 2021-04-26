#ifndef ONEGINH
#define ONEGINH

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>

/// \file

//==============================================================================================
/// String structure
struct String
{
    char* str; //< Pointer to beginning string
    size_t len; //< String length
};

/// Text structure
struct Text
{
    char* buffer; //< Array of text characters
    struct String* lines; //< Array of string structure
    size_t n_lines; //< Number of lines
    size_t size; //< Size of file
};

//==============================================================================================

/*!
Fills in the text structure
\param[in] file file - pointer to the input file
\param[in] input_text input_text - pointer to the text structure 
*/
void Create_text(FILE* file, struct Text* input_text);

/*!
Finding the file size
\param[in] file File - pointer to the input file whose size we are looking for
\return Size of file
*/
size_t Size_file(FILE* file);

/*!
Creating and filling the buffer with text characters. Also count the number of lines
\param[in] file file - pointer to the input file
\param[out] n_lines n_lines - the number of lines
\param[out] size size - size of the input file
\return Pointer to the beginning buffer with text characters
*/
char* Create_buffer(FILE* file, size_t* n_lines, size_t* size);

/*!
Placing pointers in the buffer at the beginning of lines in the text. Counting the length of these lines
\param[in] buffer buffer - pointer to the beginning buffer with text characters
\param[in] n_lines n_lines - the number of lines
\return Pointer to array of filled in string structure
*/
struct String* Placing_pointers_in_text(char* buffer, const size_t n_lines);

/*!
Comparator for comparing strings in lexicographic order from the beginning of the string
\param[in] right right - the right operand of the comparison
\param[in] right right - the right operand of the comparison
\return The result of the comparison
*/
int Comparator_direct(const void* left, const void* right);

/*!
Comparator for comparing strings in lexicographic order from the end of the string
\param[in] right right - the right operand of the comparison
\param[in] right right - the right operand of the comparison
\return The result of the comparison
*/
int Comparator_reverse(const void* left, const void* right);

/*!
Prints text to a file on the sorted pointers to strings
\param[in] file file - pointer to the output file
\param[in] input_text input_text - pointer to the text structure 
*/
void Print_text(FILE* file, const struct Text* input_text);

/*!
Prints text to a file from buffer
\param[in] file file - pointer to the output file
\param[in] input_text input_text - pointer to the text structure 
*/
void Print_buffer(FILE* file, const struct Text* input_text);

/*!
Quick sort for the strings
\param[in] strings strings - pointer to array of the string structure
\param[in] n_lines n_lines - the number of lines
\param[in] comparator comparator - pointer to comparator function
*/
void Quick_sort(struct String* strings, const size_t n_lines, int(*comparator)(const void* , const void* ));

/*!
Bubble sort for the strings
\param[in] strings strings - pointer to array of the string structure
\param[in] n_lines n_lines - the number of lines
\param[in] comparator comparator - pointer to comparator function
*/
void Bubble_sort(struct String* strings, const size_t n_lines, int(*comparator) (const void*, const void*));

/*!
Swap two lines
\param[in] right right - the right operand of the swap
\param[in] right right - the right operand of the swap
*/
void Swap(struct String* left, struct String* right);

/*!
Free dynamic memory
\param[in] text text - pointer to the text structure
*/
void Free_memory(struct Text* text);

//============================================================================================

#endif