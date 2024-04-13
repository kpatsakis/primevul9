S_join_exact(pTHX_ RExC_state_t *pRExC_state, regnode *scan,
                   UV *min_subtract, bool *unfolded_multi_char,
                   U32 flags,regnode *val, U32 depth)
{
    /* Merge several consecutive EXACTish nodes into one. */
    regnode *n = regnext(scan);
    U32 stringok = 1;
    regnode *next = scan + NODE_SZ_STR(scan);
    U32 merged = 0;
    U32 stopnow = 0;
#ifdef DEBUGGING
    regnode *stop = scan;
    GET_RE_DEBUG_FLAGS_DECL;
#else
    PERL_UNUSED_ARG(depth);
#endif

    PERL_ARGS_ASSERT_JOIN_EXACT;
#ifndef EXPERIMENTAL_INPLACESCAN
    PERL_UNUSED_ARG(flags);
    PERL_UNUSED_ARG(val);
#endif
    DEBUG_PEEP("join", scan, depth, 0);

    /* Look through the subsequent nodes in the chain.  Skip NOTHING, merge
     * EXACT ones that are mergeable to the current one. */
    while (n
           && (PL_regkind[OP(n)] == NOTHING
               || (stringok && OP(n) == OP(scan)))
           && NEXT_OFF(n)
           && NEXT_OFF(scan) + NEXT_OFF(n) < I16_MAX)
    {

        if (OP(n) == TAIL || n > next)
            stringok = 0;
        if (PL_regkind[OP(n)] == NOTHING) {
            DEBUG_PEEP("skip:", n, depth, 0);
            NEXT_OFF(scan) += NEXT_OFF(n);
            next = n + NODE_STEP_REGNODE;
#ifdef DEBUGGING
            if (stringok)
                stop = n;
#endif
            n = regnext(n);
        }
        else if (stringok) {
            const unsigned int oldl = STR_LEN(scan);
            regnode * const nnext = regnext(n);

            /* XXX I (khw) kind of doubt that this works on platforms (should
             * Perl ever run on one) where U8_MAX is above 255 because of lots
             * of other assumptions */
            /* Don't join if the sum can't fit into a single node */
            if (oldl + STR_LEN(n) > U8_MAX)
                break;

            DEBUG_PEEP("merg", n, depth, 0);
            merged++;

            NEXT_OFF(scan) += NEXT_OFF(n);
            STR_LEN(scan) += STR_LEN(n);
            next = n + NODE_SZ_STR(n);
            /* Now we can overwrite *n : */
            Move(STRING(n), STRING(scan) + oldl, STR_LEN(n), char);
#ifdef DEBUGGING
            stop = next - 1;
#endif
            n = nnext;
            if (stopnow) break;
        }

#ifdef EXPERIMENTAL_INPLACESCAN
	if (flags && !NEXT_OFF(n)) {
	    DEBUG_PEEP("atch", val, depth, 0);
	    if (reg_off_by_arg[OP(n)]) {
		ARG_SET(n, val - n);
	    }
	    else {
		NEXT_OFF(n) = val - n;
	    }
	    stopnow = 1;
	}
#endif
    }

    *min_subtract = 0;
    *unfolded_multi_char = FALSE;

    /* Here, all the adjacent mergeable EXACTish nodes have been merged.  We
     * can now analyze for sequences of problematic code points.  (Prior to
     * this final joining, sequences could have been split over boundaries, and
     * hence missed).  The sequences only happen in folding, hence for any
     * non-EXACT EXACTish node */
    if (OP(scan) != EXACT && OP(scan) != EXACTL) {
        U8* s0 = (U8*) STRING(scan);
        U8* s = s0;
        U8* s_end = s0 + STR_LEN(scan);

        int total_count_delta = 0;  /* Total delta number of characters that
                                       multi-char folds expand to */

	/* One pass is made over the node's string looking for all the
	 * possibilities.  To avoid some tests in the loop, there are two main
	 * cases, for UTF-8 patterns (which can't have EXACTF nodes) and
	 * non-UTF-8 */
	if (UTF) {
            U8* folded = NULL;

            if (OP(scan) == EXACTFL) {
                U8 *d;

                /* An EXACTFL node would already have been changed to another
                 * node type unless there is at least one character in it that
                 * is problematic; likely a character whose fold definition
                 * won't be known until runtime, and so has yet to be folded.
                 * For all but the UTF-8 locale, folds are 1-1 in length, but
                 * to handle the UTF-8 case, we need to create a temporary
                 * folded copy using UTF-8 locale rules in order to analyze it.
                 * This is because our macros that look to see if a sequence is
                 * a multi-char fold assume everything is folded (otherwise the
                 * tests in those macros would be too complicated and slow).
                 * Note that here, the non-problematic folds will have already
                 * been done, so we can just copy such characters.  We actually
                 * don't completely fold the EXACTFL string.  We skip the
                 * unfolded multi-char folds, as that would just create work
                 * below to figure out the size they already are */

                Newx(folded, UTF8_MAX_FOLD_CHAR_EXPAND * STR_LEN(scan) + 1, U8);
                d = folded;
                while (s < s_end) {
                    STRLEN s_len = UTF8SKIP(s);
                    if (! is_PROBLEMATIC_LOCALE_FOLD_utf8(s)) {
                        Copy(s, d, s_len, U8);
                        d += s_len;
                    }
                    else if (is_FOLDS_TO_MULTI_utf8(s)) {
                        *unfolded_multi_char = TRUE;
                        Copy(s, d, s_len, U8);
                        d += s_len;
                    }
                    else if (isASCII(*s)) {
                        *(d++) = toFOLD(*s);
                    }
                    else {
                        STRLEN len;
                        _toFOLD_utf8_flags(s, s_end, d, &len, FOLD_FLAGS_FULL);
                        d += len;
                    }
                    s += s_len;
                }

                /* Point the remainder of the routine to look at our temporary
                 * folded copy */
                s = folded;
                s_end = d;
            } /* End of creating folded copy of EXACTFL string */

            /* Examine the string for a multi-character fold sequence.  UTF-8
             * patterns have all characters pre-folded by the time this code is
             * executed */
            while (s < s_end - 1) /* Can stop 1 before the end, as minimum
                                     length sequence we are looking for is 2 */
	    {
                int count = 0;  /* How many characters in a multi-char fold */
                int len = is_MULTI_CHAR_FOLD_utf8_safe(s, s_end);
                if (! len) {    /* Not a multi-char fold: get next char */
                    s += UTF8SKIP(s);
                    continue;
                }

                /* Nodes with 'ss' require special handling, except for
                 * EXACTFA-ish for which there is no multi-char fold to this */
                if (len == 2 && *s == 's' && *(s+1) == 's'
                    && OP(scan) != EXACTFA
                    && OP(scan) != EXACTFA_NO_TRIE)
                {
                    count = 2;
                    if (OP(scan) != EXACTFL) {
                        OP(scan) = EXACTFU_SS;
                    }
                    s += 2;
                }
                else { /* Here is a generic multi-char fold. */
                    U8* multi_end  = s + len;

                    /* Count how many characters are in it.  In the case of
                     * /aa, no folds which contain ASCII code points are
                     * allowed, so check for those, and skip if found. */
                    if (OP(scan) != EXACTFA && OP(scan) != EXACTFA_NO_TRIE) {
                        count = utf8_length(s, multi_end);
                        s = multi_end;
                    }
                    else {
                        while (s < multi_end) {
                            if (isASCII(*s)) {
                                s++;
                                goto next_iteration;
                            }
                            else {
                                s += UTF8SKIP(s);
                            }
                            count++;
                        }
                    }
                }

                /* The delta is how long the sequence is minus 1 (1 is how long
                 * the character that folds to the sequence is) */
                total_count_delta += count - 1;
              next_iteration: ;
	    }

            /* We created a temporary folded copy of the string in EXACTFL
             * nodes.  Therefore we need to be sure it doesn't go below zero,
             * as the real string could be shorter */
            if (OP(scan) == EXACTFL) {
                int total_chars = utf8_length((U8*) STRING(scan),
                                           (U8*) STRING(scan) + STR_LEN(scan));
                if (total_count_delta > total_chars) {
                    total_count_delta = total_chars;
                }
            }

            *min_subtract += total_count_delta;
            Safefree(folded);
	}
	else if (OP(scan) == EXACTFA) {

            /* Non-UTF-8 pattern, EXACTFA node.  There can't be a multi-char
             * fold to the ASCII range (and there are no existing ones in the
             * upper latin1 range).  But, as outlined in the comments preceding
             * this function, we need to flag any occurrences of the sharp s.
             * This character forbids trie formation (because of added
             * complexity) */
#if    UNICODE_MAJOR_VERSION > 3 /* no multifolds in early Unicode */   \
   || (UNICODE_MAJOR_VERSION == 3 && (   UNICODE_DOT_VERSION > 0)       \
                                      || UNICODE_DOT_DOT_VERSION > 0)
	    while (s < s_end) {
                if (*s == LATIN_SMALL_LETTER_SHARP_S) {
                    OP(scan) = EXACTFA_NO_TRIE;
                    *unfolded_multi_char = TRUE;
                    break;
                }
                s++;
            }
        }
	else {

            /* Non-UTF-8 pattern, not EXACTFA node.  Look for the multi-char
             * folds that are all Latin1.  As explained in the comments
             * preceding this function, we look also for the sharp s in EXACTF
             * and EXACTFL nodes; it can be in the final position.  Otherwise
             * we can stop looking 1 byte earlier because have to find at least
             * two characters for a multi-fold */
	    const U8* upper = (OP(scan) == EXACTF || OP(scan) == EXACTFL)
                              ? s_end
                              : s_end -1;

	    while (s < upper) {
                int len = is_MULTI_CHAR_FOLD_latin1_safe(s, s_end);
                if (! len) {    /* Not a multi-char fold. */
                    if (*s == LATIN_SMALL_LETTER_SHARP_S
                        && (OP(scan) == EXACTF || OP(scan) == EXACTFL))
                    {
                        *unfolded_multi_char = TRUE;
                    }
                    s++;
                    continue;
                }

                if (len == 2
                    && isALPHA_FOLD_EQ(*s, 's')
                    && isALPHA_FOLD_EQ(*(s+1), 's'))
                {

                    /* EXACTF nodes need to know that the minimum length
                     * changed so that a sharp s in the string can match this
                     * ss in the pattern, but they remain EXACTF nodes, as they
                     * won't match this unless the target string is is UTF-8,
                     * which we don't know until runtime.  EXACTFL nodes can't
                     * transform into EXACTFU nodes */
                    if (OP(scan) != EXACTF && OP(scan) != EXACTFL) {
                        OP(scan) = EXACTFU_SS;
                    }
		}

                *min_subtract += len - 1;
                s += len;
	    }
#endif
	}
    }

#ifdef DEBUGGING
    /* Allow dumping but overwriting the collection of skipped
     * ops and/or strings with fake optimized ops */
    n = scan + NODE_SZ_STR(scan);
    while (n <= stop) {
	OP(n) = OPTIMIZED;
	FLAGS(n) = 0;
	NEXT_OFF(n) = 0;
        n++;
    }
#endif
    DEBUG_OPTIMISE_r(if (merged){DEBUG_PEEP("finl", scan, depth, 0);});
    return stopnow;
}