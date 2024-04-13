S_get_invlist_iter_addr(SV* invlist)
{
    /* Return the address of the UV that contains the current iteration
     * position */

    PERL_ARGS_ASSERT_GET_INVLIST_ITER_ADDR;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    return &(((XINVLIST*) SvANY(invlist))->iterator);
}