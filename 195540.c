ast_for_async_stmt(struct compiling *c, const node *n)
{
    /* async_stmt: ASYNC (funcdef | with_stmt | for_stmt) */
    REQ(n, async_stmt);
    REQ(CHILD(n, 0), ASYNC);

    switch (TYPE(CHILD(n, 1))) {
        case funcdef:
            return ast_for_funcdef_impl(c, n, NULL,
                                        true /* is_async */);
        case with_stmt:
            return ast_for_with_stmt(c, n,
                                     true /* is_async */);

        case for_stmt:
            return ast_for_for_stmt(c, n,
                                    true /* is_async */);

        default:
            PyErr_Format(PyExc_SystemError,
                         "invalid async stament: %s",
                         STR(CHILD(n, 1)));
            return NULL;
    }
}