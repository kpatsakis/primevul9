S_ssc_union(pTHX_ regnode_ssc *ssc, SV* const invlist, const bool invert2nd)
{
    PERL_ARGS_ASSERT_SSC_UNION;

    assert(is_ANYOF_SYNTHETIC(ssc));

    _invlist_union_maybe_complement_2nd(ssc->invlist,
                                        invlist,
                                        invert2nd,
                                        &ssc->invlist);
}