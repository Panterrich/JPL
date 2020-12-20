#ifndef DSL_HEADER
#define DSL_HEADER

#define PUUSH(item)     Stack_push(&proc->stk, item)
#define POOP()          Stack_pop(&proc->stk)
#define CALL_STACK      (proc->call_stk)
#define CRAM            (proc->RAM)
#define BYTECODE        (proc->bytecode)
#define REGS            (proc->registers)
#define RRAX            (proc->registers)[0]
#define RRBX            (proc->registers)[1]
#define RRCX            (proc->registers)[2]
#define RRDX            (proc->registers)[3]
#define RIP             (proc->rip)
#define SHIFT_ON_LABEL  *(size_t*)(&BYTECODE[RIP + 2])
#define CODE_MOD        BYTECODE[RIP + 1]
#define NUM_REG(shift)  BYTECODE[RIP + 2 + shift] - 'a'
#define VALUE(shift)    *(element_t*)(&BYTECODE[RIP + 2 + shift])

#endif
