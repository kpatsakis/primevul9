S_invlist_set_len(pTHX_ SV* const invlist, const UV len, const bool offset)
{
    /* Sets the current number of elements stored in the inversion list.
     * Updates SvCUR correspondingly */
    PERL_UNUSED_CONTEXT;
    PERL_ARGS_ASSERT_INVLIST_SET_LEN;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    SvCUR_set(invlist,
              (len == 0)
               ? 0
               : TO_INTERNAL_SIZE(len + offset));
    assert(SvLEN(invlist) == 0 || SvCUR(invlist) <= SvLEN(invlist));
}