S_invlist_clear(pTHX_ SV* invlist)    /* Empty the inversion list */
{
    PERL_ARGS_ASSERT_INVLIST_CLEAR;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    invlist_set_len(invlist, 0, 0);
    invlist_trim(invlist);
}