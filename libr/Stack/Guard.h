#ifndef GUARD_H
#define GUARD_H

#include "Stack.h"
//===================================================

#ifdef DOUBLE_T
    static const int code_t = 1;
    static const double Poison = NAN;
    static const char*  Poison_text = "NAN";
#endif

#ifdef INT_T
    static const int code_t = 2;
    static const int   Poison = 0xBADDED;
    static const char* Poison_text = "0xBADDED";
#endif

#ifdef CHAR_T
    static const int code_t = 3;
    static const char  Poison = '\0';
    static const char* Poison_text = "\\0";
#endif

#ifdef POINTER_T
    static const int code_t = 4;
    static void* const Poison = nullptr;
    static const char* Text_poison = "null";
#endif


//===================================================

#define STACK_ASSERT_OK(a) if (Stack_ERROR(a))                                                                         \
                           {                                                                                           \
                               FILE* log = fopen("log.txt", "a");                                                      \
                               assert(log != nullptr);                                                                 \
                               fprintf(log, "ERROR: file %s line %d function %s\n", __FILE__, __LINE__, __FUNCTION__); \
                               Stack_dump(log, a);                                                                     \
                               abort();                                                                                \
                           }
//===================================================

#define case_of_switch(enum_const) case enum_const: return #enum_const;

//===================================================

enum ERROR 
{
    SIZE_OUT_OF_CAPACITY = 1,
    OUT_OF_MEMORY,
    NEGATIVE_SIZE,
    NEGATIVE_CAPACITY,
    NULL_POINTER_TO_ARRAY,
    ARRAY_AND_STRUCTURE_POINTERS_MATCHED,
    REPEATED_CONSTRUCTION,
    WRONG_SIZE,
    NULL_POP,
    WRONG_CANARY_STRUCT_LEFT,
    WRONG_CANARY_STRUCT_RIGHT,
    WRONG_CANARY_ARRAY_LEFT,
    WRONG_CANARY_ARRAY_RIGHT,
    WRONG_STRUCT_HASH,
    WRONG_STACK_HASH,
    INVALID_PUSH,
    STACK_IS_DESTRUCTED
};

//===================================================

void Stack_null_check(struct Stack* stk);

int Stack_ERROR(struct Stack* stk);

void Stack_dump(FILE* file, struct Stack* stk);

void Poison_filling(struct Stack* stk, size_t start, size_t end);

const char* Text_ERROR(struct Stack* stk);

void Print_array(FILE*file, struct Stack* stk);

int Comparator_poison(element_t element);

void Placing_canary(struct Stack* stk, void* temp);

unsigned int Struct_stack_HASHFAQ6(struct Stack* stk);

unsigned int Stack_HASHFAQ6(struct Stack* stk);

int Stack_is_destructed(struct Stack* stk);

#endif
