S_get_invlist_previous_index_addr(SV* invlist)
{
    /* Return the address of the IV that is reserved to hold the cached index
     * */
    PERL_ARGS_ASSERT_GET_INVLIST_PREVIOUS_INDEX_ADDR;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    return &(((XINVLIST*) SvANY(invlist))->prev_index);
}