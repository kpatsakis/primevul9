ast_for_async_funcdef(struct compiling *c, const node *n, asdl_seq *decorator_seq)
{
    /* async_funcdef: ASYNC funcdef */
    REQ(n, async_funcdef);
    REQ(CHILD(n, 0), ASYNC);
    REQ(CHILD(n, 1), funcdef);

    return ast_for_funcdef_impl(c, n, decorator_seq,
                                true /* is_async */);
}