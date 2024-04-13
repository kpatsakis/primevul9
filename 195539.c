ast_for_for_stmt(struct compiling *c, const node *n0, bool is_async)
{
    const node * const n = is_async ? CHILD(n0, 1) : n0;
    asdl_seq *_target, *seq = NULL, *suite_seq;
    expr_ty expression;
    expr_ty target, first;
    const node *node_target;
    int end_lineno, end_col_offset;
    int has_type_comment;
    string type_comment;

    if (is_async && c->c_feature_version < 5) {
        ast_error(c, n,
                  "Async for loops are only supported in Python 3.5 and greater");
        return NULL;
    }

    /* for_stmt: 'for' exprlist 'in' testlist ':' [TYPE_COMMENT] suite ['else' ':' suite] */
    REQ(n, for_stmt);

    has_type_comment = TYPE(CHILD(n, 5)) == TYPE_COMMENT;

    if (NCH(n) == 9 + has_type_comment) {
        seq = ast_for_suite(c, CHILD(n, 8 + has_type_comment));
        if (!seq)
            return NULL;
    }

    node_target = CHILD(n, 1);
    _target = ast_for_exprlist(c, node_target, Store);
    if (!_target)
        return NULL;
    /* Check the # of children rather than the length of _target, since
       for x, in ... has 1 element in _target, but still requires a Tuple. */
    first = (expr_ty)asdl_seq_GET(_target, 0);
    if (NCH(node_target) == 1)
        target = first;
    else
        target = Tuple(_target, Store, first->lineno, first->col_offset,
                       node_target->n_end_lineno, node_target->n_end_col_offset,
                       c->c_arena);

    expression = ast_for_testlist(c, CHILD(n, 3));
    if (!expression)
        return NULL;
    suite_seq = ast_for_suite(c, CHILD(n, 5 + has_type_comment));
    if (!suite_seq)
        return NULL;

    if (seq != NULL) {
        get_last_end_pos(seq, &end_lineno, &end_col_offset);
    } else {
        get_last_end_pos(suite_seq, &end_lineno, &end_col_offset);
    }

    if (has_type_comment) {
        type_comment = NEW_TYPE_COMMENT(CHILD(n, 5));
        if (!type_comment)
            return NULL;
    }
    else
        type_comment = NULL;

    if (is_async)
        return AsyncFor(target, expression, suite_seq, seq, type_comment,
                        LINENO(n0), n0->n_col_offset,
                        end_lineno, end_col_offset, c->c_arena);
    else
        return For(target, expression, suite_seq, seq, type_comment,
                   LINENO(n), n->n_col_offset,
                   end_lineno, end_col_offset, c->c_arena);
}