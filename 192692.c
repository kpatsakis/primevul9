S_invlist_extend(pTHX_ SV* const invlist, const UV new_max)
{
    /* Grow the maximum size of an inversion list */

    PERL_ARGS_ASSERT_INVLIST_EXTEND;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    /* Add one to account for the zero element at the beginning which may not
     * be counted by the calling parameters */
    SvGROW((SV *)invlist, TO_INTERNAL_SIZE(new_max + 1));
}