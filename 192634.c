S_ssc_or(pTHX_ const RExC_state_t *pRExC_state, regnode_ssc *ssc,
               const regnode_charclass *or_with)
{
    /* Accumulate into SSC 'ssc' its 'OR' with 'or_with', which is either
     * another SSC or a regular ANYOF class.  Can create false positives if
     * 'or_with' is to be inverted. */

    SV* ored_cp_list;
    U8 ored_flags;

    PERL_ARGS_ASSERT_SSC_OR;

    assert(is_ANYOF_SYNTHETIC(ssc));

    /* 'or_with' is used as-is if it too is an SSC; otherwise have to extract
     * the code point inversion list and just the relevant flags */
    if (is_ANYOF_SYNTHETIC(or_with)) {
        ored_cp_list = ((regnode_ssc*) or_with)->invlist;
        ored_flags = ANYOF_FLAGS(or_with);
    }
    else {
        ored_cp_list = get_ANYOF_cp_list_for_ssc(pRExC_state, or_with);
        ored_flags = ANYOF_FLAGS(or_with) & ANYOF_COMMON_FLAGS;
        if (OP(or_with) != ANYOFD) {
            ored_flags
            |= ANYOF_FLAGS(or_with)
             & ( ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER
                |ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP);
            if (ANYOFL_UTF8_LOCALE_REQD(ANYOF_FLAGS(or_with))) {
                ored_flags |=
                    ANYOFL_SHARED_UTF8_LOCALE_fold_HAS_MATCHES_nonfold_REQD;
            }
        }
    }

    ANYOF_FLAGS(ssc) |= ored_flags;

    /* Below, C1 is the list of code points in 'ssc'; P1, its posix classes.
     * C2 is the list of code points in 'or-with'; P2, its posix classes.
     * 'or_with' may be inverted.  When not inverted, we have the simple
     * situation of computing:
     *  (C1 | P1) | (C2 | P2)  =  (C1 | C2) | (P1 | P2)
     * If P1|P2 yields a situation with both a class and its complement are
     * set, like having both \w and \W, this matches all code points, and we
     * can delete these from the P component of the ssc going forward.  XXX We
     * might be able to delete all the P components, but I (khw) am not certain
     * about this, and it is better to be safe.
     *
     * Inverted, we have
     *  (C1 | P1) | ~(C2 | P2)  =  (C1 | P1) | (~C2 & ~P2)
     *                         <=  (C1 | P1) | ~C2
     *                         <=  (C1 | ~C2) | P1
     * (which results in actually simpler code than the non-inverted case)
     * */

    if ((ANYOF_FLAGS(or_with) & ANYOF_INVERT)
        && ! is_ANYOF_SYNTHETIC(or_with))
    {
        /* We ignore P2, leaving P1 going forward */
    }   /* else  Not inverted */
    else if (ANYOF_FLAGS(or_with) & ANYOF_MATCHES_POSIXL) {
        ANYOF_POSIXL_OR((regnode_charclass_posixl*)or_with, ssc);
        if (ANYOF_POSIXL_SSC_TEST_ANY_SET(ssc)) {
            unsigned int i;
            for (i = 0; i < ANYOF_MAX; i += 2) {
                if (ANYOF_POSIXL_TEST(ssc, i) && ANYOF_POSIXL_TEST(ssc, i + 1))
                {
                    ssc_match_all_cp(ssc);
                    ANYOF_POSIXL_CLEAR(ssc, i);
                    ANYOF_POSIXL_CLEAR(ssc, i+1);
                }
            }
        }
    }

    ssc_union(ssc,
              ored_cp_list,
              FALSE /* Already has been inverted */
              );
}