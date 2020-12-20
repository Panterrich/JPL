#define LNODE               current_node->left
#define RNODE               current_node->right
#define FUNCTION            current_node->str
#define FUNC_ARG_TYPE       current_node->left->type
#define FUNC_ARG            current_node->left->str
#define OP                  (char)current_node->value
#define TYPE                current_node->type
#define PREV                current_node->prev

#define CNODE_OP(L_NODE, OPERATOR, R_NODE) Node_create_and_fill(L_NODE,                                          \
                                                                OPERATION, Name_op(OPERATOR), OPERATOR, nullptr, \
                                                                R_NODE)

#define CNODE_NUM(num) Node_create_and_fill(nullptr,                        \
                                            NUMBER, nullptr, num, nullptr,  \
                                            nullptr)

#define CNODE_FUNC(name, num, arg) Node_create_and_fill(nullptr,                  \
                                                        FUNC, name, num, nullptr, \
                                                        arg)

#define dL Derivative(tree, LNODE, var, hash_var)
#define dR Derivative(tree, RNODE, var, hash_var)
#define d(node) Derivative(tree, node, var, hash_var)

#define LCALC Evaluation(tree, current_node->left)
#define RCALC Evaluation(tree, current_node->right)