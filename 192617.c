S_ssc_add_range(pTHX_ regnode_ssc *ssc, const UV start, const UV end)
{
    PERL_ARGS_ASSERT_SSC_ADD_RANGE;

    assert(is_ANYOF_SYNTHETIC(ssc));

    ssc->invlist = _add_range_to_invlist(ssc->invlist, start, end);
}