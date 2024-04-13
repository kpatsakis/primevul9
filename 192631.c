S_ssc_finalize(pTHX_ RExC_state_t *pRExC_state, regnode_ssc *ssc)
{
    /* The inversion list in the SSC is marked mortal; now we need a more
     * permanent copy, which is stored the same way that is done in a regular
     * ANYOF node, with the first NUM_ANYOF_CODE_POINTS code points in a bit
     * map */

    SV* invlist = invlist_clone(ssc->invlist);

    PERL_ARGS_ASSERT_SSC_FINALIZE;

    assert(is_ANYOF_SYNTHETIC(ssc));

    /* The code in this file assumes that all but these flags aren't relevant
     * to the SSC, except SSC_MATCHES_EMPTY_STRING, which should be cleared
     * by the time we reach here */
    assert(! (ANYOF_FLAGS(ssc)
        & ~( ANYOF_COMMON_FLAGS
            |ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER
            |ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP)));

    populate_ANYOF_from_invlist( (regnode *) ssc, &invlist);

    set_ANYOF_arg(pRExC_state, (regnode *) ssc, invlist,
                                NULL, NULL, NULL, FALSE);

    /* Make sure is clone-safe */
    ssc->invlist = NULL;

    if (ANYOF_POSIXL_SSC_TEST_ANY_SET(ssc)) {
        ANYOF_FLAGS(ssc) |= ANYOF_MATCHES_POSIXL;
    }

    if (RExC_contains_locale) {
        OP(ssc) = ANYOFL;
    }

    assert(! (ANYOF_FLAGS(ssc) & ANYOF_LOCALE_FLAGS) || RExC_contains_locale);
}