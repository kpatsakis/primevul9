ast_for_funcdef_impl(struct compiling *c, const node *n0,
                     asdl_seq *decorator_seq, bool is_async)
{
    /* funcdef: 'def' NAME parameters ['->' test] ':' [TYPE_COMMENT] suite */
    const node * const n = is_async ? CHILD(n0, 1) : n0;
    identifier name;
    arguments_ty args;
    asdl_seq *body;
    expr_ty returns = NULL;
    int name_i = 1;
    int end_lineno, end_col_offset;
    node *tc;
    string type_comment = NULL;

    if (is_async && c->c_feature_version < 5) {
        ast_error(c, n,
                  "Async functions are only supported in Python 3.5 and greater");
        return NULL;
    }

    REQ(n, funcdef);

    name = NEW_IDENTIFIER(CHILD(n, name_i));
    if (!name)
        return NULL;
    if (forbidden_name(c, name, CHILD(n, name_i), 0))
        return NULL;
    args = ast_for_arguments(c, CHILD(n, name_i + 1));
    if (!args)
        return NULL;
    if (TYPE(CHILD(n, name_i+2)) == RARROW) {
        returns = ast_for_expr(c, CHILD(n, name_i + 3));
        if (!returns)
            return NULL;
        name_i += 2;
    }
    if (TYPE(CHILD(n, name_i + 3)) == TYPE_COMMENT) {
        type_comment = NEW_TYPE_COMMENT(CHILD(n, name_i + 3));
        if (!type_comment)
            return NULL;
        name_i += 1;
    }
    body = ast_for_suite(c, CHILD(n, name_i + 3));
    if (!body)
        return NULL;
    get_last_end_pos(body, &end_lineno, &end_col_offset);

    if (NCH(CHILD(n, name_i + 3)) > 1) {
        /* Check if the suite has a type comment in it. */
        tc = CHILD(CHILD(n, name_i + 3), 1);

        if (TYPE(tc) == TYPE_COMMENT) {
            if (type_comment != NULL) {
                ast_error(c, n, "Cannot have two type comments on def");
                return NULL;
            }
            type_comment = NEW_TYPE_COMMENT(tc);
            if (!type_comment)
                return NULL;
        }
    }

    if (is_async)
        return AsyncFunctionDef(name, args, body, decorator_seq, returns, type_comment,
                                LINENO(n0), n0->n_col_offset, end_lineno, end_col_offset, c->c_arena);
    else
        return FunctionDef(name, args, body, decorator_seq, returns, type_comment,
                           LINENO(n), n->n_col_offset, end_lineno, end_col_offset, c->c_arena);
}