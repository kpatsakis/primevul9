S_invlist_trim(SV* invlist)
{
    /* Free the not currently-being-used space in an inversion list */

    /* But don't free up the space needed for the 0 UV that is always at the
     * beginning of the list, nor the trailing NUL */
    const UV min_size = TO_INTERNAL_SIZE(1) + 1;

    PERL_ARGS_ASSERT_INVLIST_TRIM;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    SvPV_renew(invlist, MAX(min_size, SvCUR(invlist) + 1));
}