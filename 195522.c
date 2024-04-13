set_context(struct compiling *c, expr_ty e, expr_context_ty ctx, const node *n)
{
    asdl_seq *s = NULL;

    /* The ast defines augmented store and load contexts, but the
       implementation here doesn't actually use them.  The code may be
       a little more complex than necessary as a result.  It also means
       that expressions in an augmented assignment have a Store context.
       Consider restructuring so that augmented assignment uses
       set_context(), too.
    */
    assert(ctx != AugStore && ctx != AugLoad);

    switch (e->kind) {
        case Attribute_kind:
            e->v.Attribute.ctx = ctx;
            if (ctx == Store && forbidden_name(c, e->v.Attribute.attr, n, 1))
                return 0;
            break;
        case Subscript_kind:
            e->v.Subscript.ctx = ctx;
            break;
        case Starred_kind:
            e->v.Starred.ctx = ctx;
            if (!set_context(c, e->v.Starred.value, ctx, n))
                return 0;
            break;
        case Name_kind:
            if (ctx == Store) {
                if (forbidden_name(c, e->v.Name.id, n, 0))
                    return 0; /* forbidden_name() calls ast_error() */
            }
            e->v.Name.ctx = ctx;
            break;
        case List_kind:
            e->v.List.ctx = ctx;
            s = e->v.List.elts;
            break;
        case Tuple_kind:
            e->v.Tuple.ctx = ctx;
            s = e->v.Tuple.elts;
            break;
        default: {
            const char *expr_name = get_expr_name(e);
            if (expr_name != NULL) {
                ast_error(c, n, "cannot %s %s",
                          ctx == Store ? "assign to" : "delete",
                          expr_name);
            }
            return 0;
        }
    }

    /* If the LHS is a list or tuple, we need to set the assignment
       context for all the contained elements.
    */
    if (s) {
        Py_ssize_t i;

        for (i = 0; i < asdl_seq_LEN(s); i++) {
            if (!set_context(c, (expr_ty)asdl_seq_GET(s, i), ctx, n))
                return 0;
        }
    }
    return 1;
}