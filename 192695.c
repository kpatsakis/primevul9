S_ssc_is_cp_posixl_init(const RExC_state_t *pRExC_state,
                        const regnode_ssc *ssc)
{
    /* Returns TRUE if the SSC 'ssc' is in its initial state with regard only
     * to the list of code points matched, and locale posix classes; hence does
     * not check its flags) */

    UV start, end;
    bool ret;

    PERL_ARGS_ASSERT_SSC_IS_CP_POSIXL_INIT;

    assert(is_ANYOF_SYNTHETIC(ssc));

    invlist_iterinit(ssc->invlist);
    ret = invlist_iternext(ssc->invlist, &start, &end)
          && start == 0
          && end == UV_MAX;

    invlist_iterfinish(ssc->invlist);

    if (! ret) {
        return FALSE;
    }

    if (RExC_contains_locale && ! ANYOF_POSIXL_SSC_TEST_ALL_SET(ssc)) {
        return FALSE;
    }

    return TRUE;
}