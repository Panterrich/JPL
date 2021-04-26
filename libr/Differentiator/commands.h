DEF_OP(+, ADD, 41, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(-, SUB, 42, 
    {
        if (LNODE != nullptr) 
        {
            LNODE = dL; 
        }
        
        RNODE = dR;
    })

DEF_OP(*, MUL, 43, 
    {
        struct Node* l_subtree = Node_clone(LNODE, nullptr);
        struct Node* r_subtree = Node_clone(RNODE, nullptr);

        struct Node* left  = CNODE_OP(dL, MUL, r_subtree);
        struct Node* right = CNODE_OP(l_subtree,  MUL, dR);

        free(current_node->str);

        Node_fill(current_node, 
                  left,
                  OPERATION, strdup("+"), ADD, PREV,
                  right);
                
    })

DEF_OP(/, DIV, 44, 
    {
        struct Node* l_subtree   = Node_clone(LNODE, nullptr);
        struct Node* r_subtree   = Node_clone(RNODE, nullptr);
        struct Node* denominator = Node_clone(RNODE, nullptr);

        struct Node* left  = CNODE_OP(dL, MUL, r_subtree); 
        struct Node* right = CNODE_OP(l_subtree,  MUL, dR); 

        struct Node* deg   = CNODE_NUM(2);

        struct Node* sub   = CNODE_OP(left, SUB, right);
        struct Node* pow   = CNODE_OP(denominator, POW, deg);
        
        free(current_node->str);

        Node_fill(current_node,
                  sub,
                  OPERATION, strdup("/"), DIV, PREV,
                  pow);
                
    })

DEF_OP(^, POW, 45, 
    {
        int left_subtree_is_num  = Subtree_is_number(tree, LNODE);
        int right_subtree_is_num = Subtree_is_number(tree, RNODE);

        if (left_subtree_is_num && right_subtree_is_num)
        {
            Node_destruct(LNODE);
            Node_destruct(RNODE);

            free(current_node->str);

            Node_fill(current_node, 
                      nullptr,
                      NUMBER, nullptr, 0, PREV,
                      nullptr);
        }

        else if (left_subtree_is_num && !right_subtree_is_num)
        {
            struct Node* base = LNODE;
            struct Node* deg  = Node_clone(RNODE, nullptr);
            struct Node* num  = Node_clone(LNODE, nullptr);

            char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
            sprintf(name_func, "ln");

            struct Node* pow  = CNODE_OP(base, POW, deg);
            struct Node* func = CNODE_FUNC(name_func, LN, num);
            struct Node* mul  = CNODE_OP(pow, MUL, func);
            
            free(current_node->str);

            Node_fill(current_node,
                      mul,
                      OPERATION, strdup("*"), MUL, PREV,
                      dR);
        }

        else if (!left_subtree_is_num && right_subtree_is_num)
        {
            if (Is_equal(RCALC, 1))
            {   
                Re_linking_subtree(tree, current_node, dL, RNODE);
            }

            else
            {
                struct Node* base  = Node_clone(LNODE, nullptr);
                struct Node* coef  = RNODE;
                struct Node* l_deg = Node_clone(RNODE, nullptr);

                struct Node* num   = CNODE_NUM(1);
                struct Node* deg   = CNODE_OP(l_deg, SUB, num);
                struct Node* pow   = CNODE_OP(base,  POW, deg);
                struct Node* mul   = CNODE_OP(pow,   MUL, coef);
                
                free(current_node->str);

                Node_fill(current_node,
                          mul,
                          OPERATION, strdup("*"), MUL, PREV,
                          dL);
            }
        }

        else if (!left_subtree_is_num && !right_subtree_is_num) // f^g = f^g*(dg*ln(f) + g/f*df)
        {
            struct Node* f1 = Node_clone(LNODE, nullptr);
            struct Node* f2 = Node_clone(LNODE, nullptr);
            struct Node* f3 = Node_clone(LNODE, nullptr);

            struct Node* g1 = Node_clone(RNODE, nullptr);
            struct Node* g2 = Node_clone(RNODE, nullptr);

            struct Node* df = dL;
            struct Node* dg = dR;

            char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
            sprintf(name_func, "ln");

            struct Node* ln    = CNODE_FUNC(name_func, LN, f3);

            struct Node* l_mul = CNODE_OP(dg,    MUL,   ln);
            struct Node* div   = CNODE_OP(g2,    DIV,   f2);
            struct Node* r_mul = CNODE_OP(div,   MUL,   df);
            struct Node* add   = CNODE_OP(l_mul, ADD,   r_mul);
            struct Node* pow   = CNODE_OP(f1,    POW,   g1);

            free(current_node->str);
            
            Node_fill(current_node,
                      pow, 
                      OPERATION, strdup("*"), MUL, PREV,
                      add);

        }        
    })

DEF_OP(>, BAA, 46, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(<, BAB, 47, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(==, BAE, 48, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(>=, BAAE, 49, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(<=, BABE, 50, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(!=, BANE, 51, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_FUNC(ln,     LN,     0x101ba648, 65, 
    {
        struct Node* right = Node_clone(RNODE, nullptr);
                    
        free(FUNCTION);

        Node_fill(current_node,
                  dR, 
                  OPERATION, strdup("/"), DIV, PREV, 
                  right);  

    })

DEF_FUNC(sin,    SIN,    0xbf987f58, 66, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* func = CNODE_FUNC(strdup("cos"), COS, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, strdup("*"), MUL, PREV,
                  dR); 

    })

DEF_FUNC(cos,    COS,    0x238fe9,   67, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* num  = CNODE_NUM(-1);
        struct Node* func = CNODE_FUNC(strdup("sin"), SIN, arg);
        struct Node* mul  = CNODE_OP(num, MUL, func);

        free(FUNCTION);

        Node_fill(current_node,
                  mul,
                  OPERATION, strdup("*"), MUL, PREV,
                  dR);

    })

DEF_FUNC(tg,     TG,     0xf5a17ed,  68, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(strdup("cos"), COS, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);

        free(FUNCTION);

        Node_fill(current_node,
                  dR,
                  OPERATION, strdup("/"), DIV, PREV,
                  pow);

    })

DEF_FUNC(ctg,    CTG,    0xdf88c1c4, 69, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;
        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(strdup("sin"), SIN, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);
        struct Node* div  = CNODE_OP(left, DIV, pow);
        struct Node* num  = CNODE_NUM(-1);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, strdup("*"), MUL, PREV,
                  div);

    })

DEF_FUNC(arcsin, ARCSIN, 0xbf69dd95, 70, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);
        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* sub  = CNODE_OP(num1, SUB, pow);
        struct Node* func = CNODE_FUNC(strdup("sqrt"), SQRT, sub);
        
        free(FUNCTION);

        Node_fill(current_node,
                  dR,
                  OPERATION, strdup("/"), DIV, PREV,
                  func);

    })

DEF_FUNC(arccos, ARCCOS, 0x9a380fc2, 71, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;        
        struct Node* num  = CNODE_NUM(-1);
        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* sub  = CNODE_OP(num1, SUB, pow);
        struct Node* func = CNODE_FUNC(strdup("sqrt"), SQRT, sub);
        struct Node* div  = CNODE_OP(left, DIV, func);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, strdup("*"), MUL, PREV,
                  div);

    })

DEF_FUNC(arctg,  ARCTG,  0x3cffbcdb, 72, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);

        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* add  = CNODE_OP(num1, ADD, pow);

        free(FUNCTION);

        Node_fill(current_node,
                  dR,
                  OPERATION, strdup("/"), DIV, PREV,
                  add);

    })

DEF_FUNC(arcctg, ARCCTG, 0xbc7147b0, 73, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        struct Node* num  = CNODE_NUM(-1);
        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* add  = CNODE_OP(num1, ADD, pow);
        struct Node* div  = CNODE_OP(left, DIV, add);

        free(FUNCTION);

        Node_fill(current_node,
                    num,
                    OPERATION, strdup("*"), MUL, PREV,
                    div);

    })

DEF_FUNC(sh,     SH,     0x6c8057cc, 74, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* func = CNODE_FUNC(strdup("ch"), CH, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, nullptr, MUL, PREV,
                  dR);

    })

DEF_FUNC(ch,     CH,     0x4ab1a5ab, 75, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* func = CNODE_FUNC(strdup("sh"), SH, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, strdup("*"), MUL, PREV,
                  dR);

    })

DEF_FUNC(th,     TH,     0x6a4bcdcf, 76, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;
        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(strdup("ch"), CH, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);

        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, strdup("/"), DIV, PREV,
                  pow);

    })

DEF_FUNC(cth,    CTH,    0xf562ed78, 77, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;
        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(strdup("sh"), SH, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);
        struct Node* div  = CNODE_OP(left, DIV, pow);
        struct Node* num  = CNODE_NUM(-1);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, strdup("*"), MUL, PREV,
                  div);
    })

DEF_FUNC(sqrt,   SQRT,   0x145c7701, 78, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* num  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(strdup("sqrt"), SQRT, arg);
        struct Node* mul  = CNODE_OP(num, MUL, func);

        free(FUNCTION);

        Node_fill(current_node,
                  dR,
                  OPERATION, strdup("/"), DIV, PREV,
                  mul);

    })

DEF_FUNC(exp,    EXP,    0xe2313450, 79, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* func = CNODE_FUNC(strdup("exp"), EXP, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func, 
                  OPERATION, strdup("*"), MUL, PREV,
                  dR);
                  
    })
