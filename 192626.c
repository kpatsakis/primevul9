S_get_ANYOF_cp_list_for_ssc(pTHX_ const RExC_state_t *pRExC_state,
                               const regnode_charclass* const node)
{
    /* Returns a mortal inversion list defining which code points are matched
     * by 'node', which is of type ANYOF.  Handles complementing the result if
     * appropriate.  If some code points aren't knowable at this time, the
     * returned list must, and will, contain every code point that is a
     * possibility. */

    SV* invlist = NULL;
    SV* only_utf8_locale_invlist = NULL;
    unsigned int i;
    const U32 n = ARG(node);
    bool new_node_has_latin1 = FALSE;

    PERL_ARGS_ASSERT_GET_ANYOF_CP_LIST_FOR_SSC;

    /* Look at the data structure created by S_set_ANYOF_arg() */
    if (n != ANYOF_ONLY_HAS_BITMAP) {
        SV * const rv = MUTABLE_SV(RExC_rxi->data->data[n]);
        AV * const av = MUTABLE_AV(SvRV(rv));
        SV **const ary = AvARRAY(av);
        assert(RExC_rxi->data->what[n] == 's');

        if (ary[1] && ary[1] != &PL_sv_undef) { /* Has compile-time swash */
            invlist = sv_2mortal(invlist_clone(_get_swash_invlist(ary[1])));
        }
        else if (ary[0] && ary[0] != &PL_sv_undef) {

            /* Here, no compile-time swash, and there are things that won't be
             * known until runtime -- we have to assume it could be anything */
            invlist = sv_2mortal(_new_invlist(1));
            return _add_range_to_invlist(invlist, 0, UV_MAX);
        }
        else if (ary[3] && ary[3] != &PL_sv_undef) {

            /* Here no compile-time swash, and no run-time only data.  Use the
             * node's inversion list */
            invlist = sv_2mortal(invlist_clone(ary[3]));
        }

        /* Get the code points valid only under UTF-8 locales */
        if ((ANYOF_FLAGS(node) & ANYOFL_FOLD)
            && ary[2] && ary[2] != &PL_sv_undef)
        {
            only_utf8_locale_invlist = ary[2];
        }
    }

    if (! invlist) {
        invlist = sv_2mortal(_new_invlist(0));
    }

    /* An ANYOF node contains a bitmap for the first NUM_ANYOF_CODE_POINTS
     * code points, and an inversion list for the others, but if there are code
     * points that should match only conditionally on the target string being
     * UTF-8, those are placed in the inversion list, and not the bitmap.
     * Since there are circumstances under which they could match, they are
     * included in the SSC.  But if the ANYOF node is to be inverted, we have
     * to exclude them here, so that when we invert below, the end result
     * actually does include them.  (Think about "\xe0" =~ /[^\xc0]/di;).  We
     * have to do this here before we add the unconditionally matched code
     * points */
    if (ANYOF_FLAGS(node) & ANYOF_INVERT) {
        _invlist_intersection_complement_2nd(invlist,
                                             PL_UpperLatin1,
                                             &invlist);
    }

    /* Add in the points from the bit map */
    for (i = 0; i < NUM_ANYOF_CODE_POINTS; i++) {
        if (ANYOF_BITMAP_TEST(node, i)) {
            unsigned int start = i++;

            for (; i < NUM_ANYOF_CODE_POINTS && ANYOF_BITMAP_TEST(node, i); ++i) {
                /* empty */
            }
            invlist = _add_range_to_invlist(invlist, start, i-1);
            new_node_has_latin1 = TRUE;
        }
    }

    /* If this can match all upper Latin1 code points, have to add them
     * as well.  But don't add them if inverting, as when that gets done below,
     * it would exclude all these characters, including the ones it shouldn't
     * that were added just above */
    if (! (ANYOF_FLAGS(node) & ANYOF_INVERT) && OP(node) == ANYOFD
        && (ANYOF_FLAGS(node) & ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER))
    {
        _invlist_union(invlist, PL_UpperLatin1, &invlist);
    }

    /* Similarly for these */
    if (ANYOF_FLAGS(node) & ANYOF_MATCHES_ALL_ABOVE_BITMAP) {
        _invlist_union_complement_2nd(invlist, PL_InBitmap, &invlist);
    }

    if (ANYOF_FLAGS(node) & ANYOF_INVERT) {
        _invlist_invert(invlist);
    }
    else if (new_node_has_latin1 && ANYOF_FLAGS(node) & ANYOFL_FOLD) {

        /* Under /li, any 0-255 could fold to any other 0-255, depending on the
         * locale.  We can skip this if there are no 0-255 at all. */
        _invlist_union(invlist, PL_Latin1, &invlist);
    }

    /* Similarly add the UTF-8 locale possible matches.  These have to be
     * deferred until after the non-UTF-8 locale ones are taken care of just
     * above, or it leads to wrong results under ANYOF_INVERT */
    if (only_utf8_locale_invlist) {
        _invlist_union_maybe_complement_2nd(invlist,
                                            only_utf8_locale_invlist,
                                            ANYOF_FLAGS(node) & ANYOF_INVERT,
                                            &invlist);
    }

    return invlist;
}