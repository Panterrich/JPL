#include "Guard.h"
//===================================================

void Stack_construct(struct Stack* stk, long capacity)
{
    Stack_null_check(stk);

    if ((stk->canary_struct_left != 0) || (stk->canary_struct_right != 0))
    {   
        if (!Stack_is_destructed(stk))
        {   
            if (stk->struct_hash != Struct_stack_HASHFAQ6(stk))
            {
                stk->error = WRONG_STRUCT_HASH;
                STACK_ASSERT_OK(stk);
            }

            else
            {   
                Stack_destruct(stk);
            }
        }
    }

    if (capacity < 0)
    {   
        stk->error = NEGATIVE_CAPACITY;
        STACK_ASSERT_OK(stk);
    }

    if (capacity == 0)
    {
        stk->capacity = (size_t)capacity;
        stk->canary_struct_left  = Canary;
        stk->canary_struct_right = Canary;
        stk->size  = 0;
        stk->error = 0;
        stk->data = nullptr;

        stk->struct_hash = Struct_stack_HASHFAQ6(stk);
        stk->stack_hash  = Stack_HASHFAQ6(stk);
    }

    else
    {   
        stk->capacity = (size_t)capacity;
        stk->canary_struct_left  = Canary;
        stk->canary_struct_right = Canary;
        stk->struct_hash = 0;
        stk->stack_hash  = 0;

        void* temp = malloc((stk->capacity) * sizeof(element_t) + 2 * sizeof(canary_t));

        if (temp == nullptr)
        {
            stk->error = OUT_OF_MEMORY;
            STACK_ASSERT_OK(stk);
        }

        Placing_canary(stk, temp);
        
        stk->size  = 0;
        stk->error = 0;

        Poison_filling(stk, stk->size, stk->capacity);

        stk->struct_hash = Struct_stack_HASHFAQ6(stk);
        stk->stack_hash  = Stack_HASHFAQ6(stk);
    }
}

void Stack_push(struct Stack* stk, element_t element)
{
    Stack_null_check(stk);
    STACK_ASSERT_OK(stk);

    Stack_reallocation_memory(stk);

    STACK_ASSERT_OK(stk);

    if (Comparator_poison(element))
    {
        stk->error = INVALID_PUSH;
        STACK_ASSERT_OK(stk);
    }

    stk->data[(stk->size)++] = element;

    stk->struct_hash = Struct_stack_HASHFAQ6(stk);
    stk->stack_hash  = Stack_HASHFAQ6(stk);

    STACK_ASSERT_OK(stk);
}

element_t Stack_pop(struct Stack* stk)
{
    Stack_null_check(stk);
    STACK_ASSERT_OK(stk);

    Stack_reverse_reallocation_memory(stk);

    STACK_ASSERT_OK(stk);

    element_t temp = Poison;

    if (stk->size == 0)
    {
        stk->error = NULL_POP;
        STACK_ASSERT_OK(stk);
    }

    temp = stk->data[--(stk->size)];
    stk->data[stk->size] = Poison;

    stk->struct_hash = Struct_stack_HASHFAQ6(stk);
    stk->stack_hash  = Stack_HASHFAQ6(stk);

    return temp;
}

void Stack_reallocation_memory(struct Stack* stk)
{
    Stack_null_check(stk);

    if (stk->capacity == 0)
    {   
        void* temp = (void*) stk->data;

        temp = malloc(2 * sizeof(element_t) + 2 * sizeof(canary_t));
                
        if (temp == nullptr)
        {
            stk->error = OUT_OF_MEMORY; 
        }

        stk->capacity += 2;

        Placing_canary(stk, temp);

        Poison_filling(stk, stk->size, stk->capacity);

        stk->struct_hash = Struct_stack_HASHFAQ6(stk);
        stk->stack_hash  = Stack_HASHFAQ6(stk);
    }

    if (stk->size == (stk->capacity - 1))
    {
        void* temp = (void*) stk->data;

        temp = realloc(&((canary_t*) stk->data)[-1], 2 * stk->capacity * sizeof(element_t) + 2 * sizeof(canary_t));

        if (temp == nullptr)
        {
            temp = realloc(&((canary_t*)stk->data)[-1], (size_t)(1.5 * (double)stk->capacity * sizeof(element_t) + 2 * sizeof(canary_t)));

            if (temp == nullptr)
            {
                temp = realloc(&((canary_t*)stk->data)[-1], (stk->capacity + 1) * sizeof(element_t) + 2 * sizeof(canary_t));
                
                if (temp == nullptr)
                {
                  stk->error = OUT_OF_MEMORY;
                  STACK_ASSERT_OK(stk);
                }
                
                else
                {   
                    stk->capacity += 1;

                    Placing_canary(stk, temp);

                    stk->data[stk->capacity - 1] = Poison;

                    stk->struct_hash = Struct_stack_HASHFAQ6(stk);
                    stk->stack_hash  = Stack_HASHFAQ6(stk);
                }

            }

            else
            {   
                stk->capacity = (size_t)(1.5 * (double)stk->capacity);

                Placing_canary(stk, temp);
                Poison_filling(stk, stk->size + 1, stk->capacity);

                stk->struct_hash = Struct_stack_HASHFAQ6(stk);
                stk->stack_hash  = Stack_HASHFAQ6(stk);
            }
            
        }
        
        else
        {   
            stk->capacity *= 2;

            Placing_canary(stk, temp);
            Poison_filling(stk, stk->size + 1, stk->capacity);

            stk->struct_hash = Struct_stack_HASHFAQ6(stk);
            stk->stack_hash  = Stack_HASHFAQ6(stk);
        }
    }
}

void Stack_reverse_reallocation_memory(struct Stack* stk)
{
    Stack_null_check(stk);

    if ((stk->capacity >= 4) && (stk->size < ((stk->capacity) / 4)))
    {
        stk->capacity /= 4;

        void* temp = realloc(&((canary_t*)stk->data)[-1], (stk->capacity + 1) * sizeof(element_t) + 2 * sizeof(canary_t));

        Placing_canary(stk, temp);

        stk->struct_hash = Struct_stack_HASHFAQ6(stk);
        stk->stack_hash  = Stack_HASHFAQ6(stk);
    }

    STACK_ASSERT_OK(stk);
}

void Stack_destruct(struct Stack* stk)
{
    Stack_null_check(stk);

    if (stk->data != nullptr) free(&(((canary_t*)stk->data)[-1]));

    stk->data  = nullptr;
    stk->size  = (size_t)-1;
    stk->error =  0;
    stk->capacity    = (size_t)-1;
    stk->struct_hash =  0;
    stk->stack_hash  =  0;
    stk->canary_struct_left  = (canary_t)-1;
    stk->canary_struct_right = (canary_t)-1;
}
