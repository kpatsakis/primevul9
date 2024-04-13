S_invlist_max(SV* const invlist)
{
    /* Returns the maximum number of elements storable in the inversion list's
     * array, without having to realloc() */

    PERL_ARGS_ASSERT_INVLIST_MAX;

    assert(SvTYPE(invlist) == SVt_INVLIST);

    /* Assumes worst case, in which the 0 element is not counted in the
     * inversion list, so subtracts 1 for that */
    return SvLEN(invlist) == 0  /* This happens under _new_invlist_C_array */
           ? FROM_INTERNAL_SIZE(SvCUR(invlist)) - 1
           : FROM_INTERNAL_SIZE(SvLEN(invlist)) - 1;
}