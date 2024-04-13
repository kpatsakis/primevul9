ast_for_binop(struct compiling *c, const node *n)
{
    /* Must account for a sequence of expressions.
       How should A op B op C by represented?
       BinOp(BinOp(A, op, B), op, C).
    */

    int i, nops;
    expr_ty expr1, expr2, result;
    operator_ty newoperator;

    expr1 = ast_for_expr(c, CHILD(n, 0));
    if (!expr1)
        return NULL;

    expr2 = ast_for_expr(c, CHILD(n, 2));
    if (!expr2)
        return NULL;

    newoperator = get_operator(c, CHILD(n, 1));
    if (!newoperator)
        return NULL;

    result = BinOp(expr1, newoperator, expr2, LINENO(n), n->n_col_offset,
                   CHILD(n, 2)->n_end_lineno, CHILD(n, 2)->n_end_col_offset,
                   c->c_arena);
    if (!result)
        return NULL;

    nops = (NCH(n) - 1) / 2;
    for (i = 1; i < nops; i++) {
        expr_ty tmp_result, tmp;
        const node* next_oper = CHILD(n, i * 2 + 1);

        newoperator = get_operator(c, next_oper);
        if (!newoperator)
            return NULL;

        tmp = ast_for_expr(c, CHILD(n, i * 2 + 2));
        if (!tmp)
            return NULL;

        tmp_result = BinOp(result, newoperator, tmp,
                           LINENO(next_oper), next_oper->n_col_offset,
                           CHILD(n, i * 2 + 2)->n_end_lineno,
                           CHILD(n, i * 2 + 2)->n_end_col_offset,
                           c->c_arena);
        if (!tmp_result)
            return NULL;
        result = tmp_result;
    }
    return result;
}