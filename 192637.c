S_ssc_is_anything(const regnode_ssc *ssc)
{
    /* Returns TRUE if the SSC 'ssc' can match the empty string and any code
     * point; FALSE otherwise.  Thus, this is used to see if using 'ssc' buys
     * us anything: if the function returns TRUE, 'ssc' hasn't been restricted
     * in any way, so there's no point in using it */

    UV start, end;
    bool ret;

    PERL_ARGS_ASSERT_SSC_IS_ANYTHING;

    assert(is_ANYOF_SYNTHETIC(ssc));

    if (! (ANYOF_FLAGS(ssc) & SSC_MATCHES_EMPTY_STRING)) {
        return FALSE;
    }

    /* See if the list consists solely of the range 0 - Infinity */
    invlist_iterinit(ssc->invlist);
    ret = invlist_iternext(ssc->invlist, &start, &end)
          && start == 0
          && end == UV_MAX;

    invlist_iterfinish(ssc->invlist);

    if (ret) {
        return TRUE;
    }

    /* If e.g., both \w and \W are set, matches everything */
    if (ANYOF_POSIXL_SSC_TEST_ANY_SET(ssc)) {
        int i;
        for (i = 0; i < ANYOF_POSIXL_MAX; i += 2) {
            if (ANYOF_POSIXL_TEST(ssc, i) && ANYOF_POSIXL_TEST(ssc, i+1)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}