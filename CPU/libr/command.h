DEF_COMMAND(HLT,  0,  0x1e3edd86, 0, 
    {
        return 1;
    })

DEF_COMMAND(DUMP, 7,  0x39c100dd, 0, 
    {
        Stack_dump(stdout, &(proc->stk));
        Stack_dump(stdout, &(proc->call_stk));

        RIP += size_cmd;
    })

DEF_COMMAND(PURGE, 4, 0x6fee0dcd, 1, 
    {
        switch(CODE_MOD)
        {
            case MOD_EMPTY:
                for (int i = 0; i < 4; ++i)
                {
                    REGS[i] = Poison;
                }

                RIP += size_cmd + size_mod;
                break;

            case MOD_00R:
                REGS[NUM_REG(0)] = Poison;

                RIP += size_cmd + size_mod + size_reg;
                break;
        }
    })

DEF_COMMAND(IN,   10, 0x2d0b8777, 1, 
    {   
        printf("IN: ");

        element_t element = 0;
        scanf("%lg", &element);

        switch(CODE_MOD)
        {
            case MOD_EMPTY:
                PUUSH(element);

                RIP += size_cmd + size_mod;
                break;

            case MOD_00R:

                REGS[NUM_REG(0)] = element;

                RIP += size_cmd + size_mod + size_reg;
                break;
        }
    })

DEF_COMMAND(OUT,  11, 0xcd3efa96, 1, 
    {   
        switch(CODE_MOD)
        {
            case MOD_EMPTY:
                printf("OUT: %lg \n", POOP());

                RIP += size_cmd + size_mod;
                break;

            case MOD_00R:
                printf("Out: R%cX = %lg \n", NUM_REG(0) + 'A', REGS[NUM_REG(0)]);

                RIP += size_cmd + size_mod + size_reg;
                break;
        }
    })

DEF_COMMAND(PUSH, 20, 0x12e2afe4, 1, 
    {
        switch(BYTECODE[RIP + 1])
        {
            case MOD_RAR:
                PUUSH(CRAM[(int)(VALUE(0) + REGS[NUM_REG(sizeof(element_t))])]);

                RIP += size_cmd + size_mod + size_arg + size_reg;
                break;

            case MOD_R0R:
                PUUSH(CRAM[(int)REGS[NUM_REG(0)]]);

                RIP += size_cmd + size_mod + size_reg;
                break;
            
            case MOD_RA0:
                PUUSH(CRAM[(int)VALUE(0)]);

                RIP += size_cmd + size_mod + size_arg;
                break;

            case MOD_00R:
                PUUSH(REGS[NUM_REG(0)]);

                RIP += size_cmd + size_mod + size_reg;
                break;

            case MOD_0A0:
                PUUSH(VALUE(0));

                RIP += size_cmd + size_mod + size_arg; 
                break;
            
            case MOD_0AR:
                PUUSH(VALUE(0) + REGS[NUM_REG(sizeof(element_t))]);

                RIP += size_cmd + size_mod + size_arg + size_reg; 
                break;

        }
    })

DEF_COMMAND(POP,  21, 0xa9527a55, 1, 
    {
        switch(CODE_MOD)
        {
            case MOD_EMPTY:
                POOP();

                RIP += size_cmd + size_mod;
                break;

            case MOD_RAR:
                CRAM[(int)(VALUE(0) + REGS[NUM_REG(sizeof(element_t))])] = POOP();

                RIP += size_cmd + size_mod + size_arg + size_reg;
                break;

            case MOD_R0R:
                CRAM[(int)REGS[NUM_REG(0)]] = POOP();

                RIP += size_cmd + size_mod + size_reg;
                break;
            
            case MOD_RA0:
                CRAM[(int)VALUE(0)] = POOP();

                RIP += size_cmd + size_mod + size_arg;
                break;

            case MOD_00R:
                REGS[NUM_REG(0)] = POOP();

                RIP += size_cmd + size_mod + size_reg;
                break;
        }
    })

DEF_COMMAND(JMP, 30, 0xed5354f0, 1, 
    {
        RIP = SHIFT_ON_LABEL;
    })

DEF_COMMAND(JA, 31, 0x5d53e6f3, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 > temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(JAE, 32, 0xfa010a1d, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 >= temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(JB, 33, 0x15dbd7ec, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 < temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(JBE, 34, 0x70796a7c, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 <= temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(JE, 35, 0x4326b285, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 == temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(JNE, 36, 0x9cce3533, 1, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();

        if (temp2 != temp1)
        {
            RIP = SHIFT_ON_LABEL;
        }
        else 
        {
            RIP += size_cmd + size_mod + size_lbl;
        }
    })

DEF_COMMAND(CALL, 37, 0xe3ed334c, 1, 
    {
        Stack_push(&CALL_STACK, (RIP + size_cmd + size_mod + size_lbl));

        RIP = SHIFT_ON_LABEL;

    })

DEF_COMMAND(RET, 38, 0x438f69b8, 0,
    {
        RIP = Stack_pop(&CALL_STACK);
    })

DEF_COMMAND(NEG,  40, 0x3b1a041e, 0, 
    {
        element_t temp = POOP();
        PUUSH(-temp);

        RIP += size_cmd;
    })

DEF_COMMAND(ADD,  41, 0xc281d6b4, 0, 
    {
        element_t temp_1 = POOP();
        element_t temp_2 = POOP();
        PUUSH(temp_1 + temp_2);

        RIP += size_cmd;
    })
    
DEF_COMMAND(SUB,  42, 0x1aa20312, 0, 
    {
        element_t temp_1 = POOP();
        element_t temp_2 = POOP();
        PUUSH(temp_2 - temp_1);

        RIP += size_cmd;
    })

DEF_COMMAND(MUL,  43, 0xadd0b179, 0, 
    {
        element_t temp_1 = POOP();
        element_t temp_2 = POOP();
        PUUSH(temp_1 * temp_2);

        RIP += size_cmd;
    })

DEF_COMMAND(DIV,  44, 0xe550a8d4, 0, 
    {
        element_t temp_1 = POOP();
        element_t temp_2 = POOP();
        PUUSH(temp_2 / temp_1);

        RIP += size_cmd;
    })

DEF_COMMAND(POW,  45, 0xe7a3f707, 0, 
    {
        element_t temp_1 = POOP();
        element_t temp_2 = POOP();
        PUUSH(pow(temp_2, temp_1));

        RIP += size_cmd;
    })

DEF_COMMAND(BAA, 46, 0xb4a71050, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 > temp1);
        
        RIP += size_cmd;
    })

DEF_COMMAND(BAAE, 47, 0xfa7ac3f6, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 >= temp1);

        RIP += size_cmd;
    })

DEF_COMMAND(BAB, 48, 0x261d733b, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 < temp1);

        RIP += size_cmd;
    })

DEF_COMMAND(BABE, 49, 0x85dddba2, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 <= temp1);

        RIP += size_cmd;
    })

DEF_COMMAND(BAE, 50, 0xfbae1e5d, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 == temp1);

        RIP += size_cmd;
    })

DEF_COMMAND(BANE, 51, 0x4bc1d7eb, 0, 
    {
        element_t temp1 = POOP();
        element_t temp2 = POOP();
        PUUSH(temp2 != temp1);

        RIP += size_cmd;
    })

DEF_COMMAND(LN,  65, 0x9df6e5e8, 0, 
    {
        element_t temp = POOP();
        PUUSH(log(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(SIN,  66, 0xab2b99ba, 0, 
    {
        element_t temp = POOP();
        PUUSH(sin(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(COS,  67, 0x371862fa, 0, 
    {
        element_t temp = POOP();
        PUUSH(cos(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(TG,   68, 0xeda6fc83, 0, 
    {
        element_t temp = POOP();
        PUUSH(tan(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(CTG,  69, 0x3bd5719c, 0, 
    {
        element_t temp = POOP();
        PUUSH(1.0 / tan(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(ARCSIN,  70, 0xd25d8173, 0, 
    {
        element_t temp = POOP();
        PUUSH(asin(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(ARCCOS,  71, 0xaf22d6a6, 0, 
    {
        element_t temp = POOP();
        PUUSH(acos(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(ARCTG,  72, 0x5d5cafca, 0, 
    {
        element_t temp = POOP();
        PUUSH(atan(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(ARCCTG,  73, 0x6fd9be41, 0, 
    {
        element_t temp = POOP();
        PUUSH(M_PI_2 - atan(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(SH,  74, 0xcad33810, 0, 
    {
        element_t temp = POOP();
        PUUSH(sinh(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(CH,  75, 0x73742d7, 0, 
    {
        element_t temp = POOP();
        PUUSH(cosh(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(TH, 76, 0x1483ca3c, 0, 
    {
        element_t temp = POOP();
        PUUSH(tanh(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(CTH,  77, 0x307f5af0, 0, 
    {
        element_t temp = POOP();
        PUUSH(1.0 / tan(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(SQRT, 78, 0x9cca14cb, 0, 
    {
        element_t temp = POOP();
        PUUSH(sqrt(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(EXP,  79, 0xa5b77910, 0, 
    {
        element_t temp = POOP();
        PUUSH(exp(temp));

        RIP += size_cmd;
    })

DEF_COMMAND(SCREEN, 80, 0x31ef5aa8, 0,
    {
        printf("@cls||clear");
        
        for (int y = 0; y < 200; y += 2)
        {
            for (int x = 0; x < 200; ++x)
            {
                if (*((proc->RAM + 1000) + x + y * 200) == 1)
                {
                    printf("\x1b[1;34m@\x1b[0m");
                }
                
                else
                {
                    printf("\x1b[8;40mE\x1b[0m");
                }
            }
            
            printf("\n");
        }

        //for (size_t i = 0; i < 999999; ++i);

        RIP += size_cmd;
    })

DEF_COMMAND(MEMRES, 81, 0x97444e3d, 0, {

    for (int y = 0; y < 200; ++y)
    {
        for (int x = 0; x < 200; ++x)
        {
            *((proc->RAM + 1000) + x + y * 200) = 0;    
        }
    }

    RIP += size_cmd;
})


DEF_MOD(MOD_EMPTY,   0)
DEF_MOD(MOD_RAR, 0b111)
DEF_MOD(MOD_RA0, 0b110)
DEF_MOD(MOD_R0R, 0b101)
DEF_MOD(MOD_0AR, 0b011)
DEF_MOD(MOD_0A0, 0b010)
DEF_MOD(MOD_00R, 0b001)
DEF_MOD(MOD_POINTER, 10)


DEF_REG(RAX, 97,  0xf49435a3)
DEF_REG(RBX, 98,  0x4b98744e)
DEF_REG(RCX, 99,  0xfb88d770)
DEF_REG(RDX, 100, 0xb178c235)
DEF_REG(REX, 101, 0xcf2300e1)
DEF_REG(RFX, 102, 0x8fc280dd)