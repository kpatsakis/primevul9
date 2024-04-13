S_ssc_anything(pTHX_ regnode_ssc *ssc)
{
    /* Set the SSC 'ssc' to match an empty string or any code point */

    PERL_ARGS_ASSERT_SSC_ANYTHING;

    assert(is_ANYOF_SYNTHETIC(ssc));

    /* mortalize so won't leak */
    ssc->invlist = sv_2mortal(_add_range_to_invlist(NULL, 0, UV_MAX));
    ANYOF_FLAGS(ssc) |= SSC_MATCHES_EMPTY_STRING;  /* Plus matches empty */
}