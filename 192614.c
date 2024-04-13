S__make_exactf_invlist(pTHX_ RExC_state_t *pRExC_state, regnode *node)
{
    const U8 * s = (U8*)STRING(node);
    SSize_t bytelen = STR_LEN(node);
    UV uc;
    /* Start out big enough for 2 separate code points */
    SV* invlist = _new_invlist(4);

    PERL_ARGS_ASSERT__MAKE_EXACTF_INVLIST;

    if (! UTF) {
        uc = *s;

        /* We punt and assume can match anything if the node begins
         * with a multi-character fold.  Things are complicated.  For
         * example, /ffi/i could match any of:
         *  "\N{LATIN SMALL LIGATURE FFI}"
         *  "\N{LATIN SMALL LIGATURE FF}I"
         *  "F\N{LATIN SMALL LIGATURE FI}"
         *  plus several other things; and making sure we have all the
         *  possibilities is hard. */
        if (is_MULTI_CHAR_FOLD_latin1_safe(s, s + bytelen)) {
            invlist = _add_range_to_invlist(invlist, 0, UV_MAX);
        }
        else {
            /* Any Latin1 range character can potentially match any
             * other depending on the locale */
            if (OP(node) == EXACTFL) {
                _invlist_union(invlist, PL_Latin1, &invlist);
            }
            else {
                /* But otherwise, it matches at least itself.  We can
                 * quickly tell if it has a distinct fold, and if so,
                 * it matches that as well */
                invlist = add_cp_to_invlist(invlist, uc);
                if (IS_IN_SOME_FOLD_L1(uc))
                    invlist = add_cp_to_invlist(invlist, PL_fold_latin1[uc]);
            }

            /* Some characters match above-Latin1 ones under /i.  This
             * is true of EXACTFL ones when the locale is UTF-8 */
            if (HAS_NONLATIN1_SIMPLE_FOLD_CLOSURE(uc)
                && (! isASCII(uc) || (OP(node) != EXACTFA
                                    && OP(node) != EXACTFA_NO_TRIE)))
            {
                add_above_Latin1_folds(pRExC_state, (U8) uc, &invlist);
            }
        }
    }
    else {  /* Pattern is UTF-8 */
        U8 folded[UTF8_MAX_FOLD_CHAR_EXPAND * UTF8_MAXBYTES_CASE + 1] = { '\0' };
        STRLEN foldlen = UTF8SKIP(s);
        const U8* e = s + bytelen;
        SV** listp;

        uc = utf8_to_uvchr_buf(s, s + bytelen, NULL);

        /* The only code points that aren't folded in a UTF EXACTFish
         * node are are the problematic ones in EXACTFL nodes */
        if (OP(node) == EXACTFL && is_PROBLEMATIC_LOCALE_FOLDEDS_START_cp(uc)) {
            /* We need to check for the possibility that this EXACTFL
             * node begins with a multi-char fold.  Therefore we fold
             * the first few characters of it so that we can make that
             * check */
            U8 *d = folded;
            int i;

            for (i = 0; i < UTF8_MAX_FOLD_CHAR_EXPAND && s < e; i++) {
                if (isASCII(*s)) {
                    *(d++) = (U8) toFOLD(*s);
                    s++;
                }
                else {
                    STRLEN len;
                    toFOLD_utf8_safe(s, e, d, &len);
                    d += len;
                    s += UTF8SKIP(s);
                }
            }

            /* And set up so the code below that looks in this folded
             * buffer instead of the node's string */
            e = d;
            foldlen = UTF8SKIP(folded);
            s = folded;
        }

        /* When we reach here 's' points to the fold of the first
         * character(s) of the node; and 'e' points to far enough along
         * the folded string to be just past any possible multi-char
         * fold. 'foldlen' is the length in bytes of the first
         * character in 's'
         *
         * Unlike the non-UTF-8 case, the macro for determining if a
         * string is a multi-char fold requires all the characters to
         * already be folded.  This is because of all the complications
         * if not.  Note that they are folded anyway, except in EXACTFL
         * nodes.  Like the non-UTF case above, we punt if the node
         * begins with a multi-char fold  */

        if (is_MULTI_CHAR_FOLD_utf8_safe(s, e)) {
            invlist = _add_range_to_invlist(invlist, 0, UV_MAX);
        }
        else {  /* Single char fold */

            /* It matches all the things that fold to it, which are
             * found in PL_utf8_foldclosures (including itself) */
            invlist = add_cp_to_invlist(invlist, uc);
            if (! PL_utf8_foldclosures)
                _load_PL_utf8_foldclosures();
            if ((listp = hv_fetch(PL_utf8_foldclosures,
                                (char *) s, foldlen, FALSE)))
            {
                AV* list = (AV*) *listp;
                IV k;
                for (k = 0; k <= av_tindex_skip_len_mg(list); k++) {
                    SV** c_p = av_fetch(list, k, FALSE);
                    UV c;
                    assert(c_p);

                    c = SvUV(*c_p);

                    /* /aa doesn't allow folds between ASCII and non- */
                    if ((OP(node) == EXACTFA || OP(node) == EXACTFA_NO_TRIE)
                        && isASCII(c) != isASCII(uc))
                    {
                        continue;
                    }

                    invlist = add_cp_to_invlist(invlist, c);
                }
            }
        }
    }

    return invlist;
}