S_ssc_and(pTHX_ const RExC_state_t *pRExC_state, regnode_ssc *ssc,
                const regnode_charclass *and_with)
{
    /* Accumulate into SSC 'ssc' its 'AND' with 'and_with', which is either
     * another SSC or a regular ANYOF class.  Can create false positives. */

    SV* anded_cp_list;
    U8  anded_flags;

    PERL_ARGS_ASSERT_SSC_AND;

    assert(is_ANYOF_SYNTHETIC(ssc));

    /* 'and_with' is used as-is if it too is an SSC; otherwise have to extract
     * the code point inversion list and just the relevant flags */
    if (is_ANYOF_SYNTHETIC(and_with)) {
        anded_cp_list = ((regnode_ssc *)and_with)->invlist;
        anded_flags = ANYOF_FLAGS(and_with);

        /* XXX This is a kludge around what appears to be deficiencies in the
         * optimizer.  If we make S_ssc_anything() add in the WARN_SUPER flag,
         * there are paths through the optimizer where it doesn't get weeded
         * out when it should.  And if we don't make some extra provision for
         * it like the code just below, it doesn't get added when it should.
         * This solution is to add it only when AND'ing, which is here, and
         * only when what is being AND'ed is the pristine, original node
         * matching anything.  Thus it is like adding it to ssc_anything() but
         * only when the result is to be AND'ed.  Probably the same solution
         * could be adopted for the same problem we have with /l matching,
         * which is solved differently in S_ssc_init(), and that would lead to
         * fewer false positives than that solution has.  But if this solution
         * creates bugs, the consequences are only that a warning isn't raised
         * that should be; while the consequences for having /l bugs is
         * incorrect matches */
        if (ssc_is_anything((regnode_ssc *)and_with)) {
            anded_flags |= ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER;
        }
    }
    else {
        anded_cp_list = get_ANYOF_cp_list_for_ssc(pRExC_state, and_with);
        if (OP(and_with) == ANYOFD) {
            anded_flags = ANYOF_FLAGS(and_with) & ANYOF_COMMON_FLAGS;
        }
        else {
            anded_flags = ANYOF_FLAGS(and_with)
            &( ANYOF_COMMON_FLAGS
              |ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER
              |ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP);
            if (ANYOFL_UTF8_LOCALE_REQD(ANYOF_FLAGS(and_with))) {
                anded_flags &=
                    ANYOFL_SHARED_UTF8_LOCALE_fold_HAS_MATCHES_nonfold_REQD;
            }
        }
    }

    ANYOF_FLAGS(ssc) &= anded_flags;

    /* Below, C1 is the list of code points in 'ssc'; P1, its posix classes.
     * C2 is the list of code points in 'and-with'; P2, its posix classes.
     * 'and_with' may be inverted.  When not inverted, we have the situation of
     * computing:
     *  (C1 | P1) & (C2 | P2)
     *                     =  (C1 & (C2 | P2)) | (P1 & (C2 | P2))
     *                     =  ((C1 & C2) | (C1 & P2)) | ((P1 & C2) | (P1 & P2))
     *                    <=  ((C1 & C2) |       P2)) | ( P1       | (P1 & P2))
     *                    <=  ((C1 & C2) | P1 | P2)
     * Alternatively, the last few steps could be:
     *                     =  ((C1 & C2) | (C1 & P2)) | ((P1 & C2) | (P1 & P2))
     *                    <=  ((C1 & C2) |  C1      ) | (      C2  | (P1 & P2))
     *                    <=  (C1 | C2 | (P1 & P2))
     * We favor the second approach if either P1 or P2 is non-empty.  This is
     * because these components are a barrier to doing optimizations, as what
     * they match cannot be known until the moment of matching as they are
     * dependent on the current locale, 'AND"ing them likely will reduce or
     * eliminate them.
     * But we can do better if we know that C1,P1 are in their initial state (a
     * frequent occurrence), each matching everything:
     *  (<everything>) & (C2 | P2) =  C2 | P2
     * Similarly, if C2,P2 are in their initial state (again a frequent
     * occurrence), the result is a no-op
     *  (C1 | P1) & (<everything>) =  C1 | P1
     *
     * Inverted, we have
     *  (C1 | P1) & ~(C2 | P2)  =  (C1 | P1) & (~C2 & ~P2)
     *                          =  (C1 & (~C2 & ~P2)) | (P1 & (~C2 & ~P2))
     *                         <=  (C1 & ~C2) | (P1 & ~P2)
     * */

    if ((ANYOF_FLAGS(and_with) & ANYOF_INVERT)
        && ! is_ANYOF_SYNTHETIC(and_with))
    {
        unsigned int i;

        ssc_intersection(ssc,
                         anded_cp_list,
                         FALSE /* Has already been inverted */
                         );

        /* If either P1 or P2 is empty, the intersection will be also; can skip
         * the loop */
        if (! (ANYOF_FLAGS(and_with) & ANYOF_MATCHES_POSIXL)) {
            ANYOF_POSIXL_ZERO(ssc);
        }
        else if (ANYOF_POSIXL_SSC_TEST_ANY_SET(ssc)) {

            /* Note that the Posix class component P from 'and_with' actually
             * looks like:
             *      P = Pa | Pb | ... | Pn
             * where each component is one posix class, such as in [\w\s].
             * Thus
             *      ~P = ~(Pa | Pb | ... | Pn)
             *         = ~Pa & ~Pb & ... & ~Pn
             *        <= ~Pa | ~Pb | ... | ~Pn
             * The last is something we can easily calculate, but unfortunately
             * is likely to have many false positives.  We could do better
             * in some (but certainly not all) instances if two classes in
             * P have known relationships.  For example
             *      :lower: <= :alpha: <= :alnum: <= \w <= :graph: <= :print:
             * So
             *      :lower: & :print: = :lower:
             * And similarly for classes that must be disjoint.  For example,
             * since \s and \w can have no elements in common based on rules in
             * the POSIX standard,
             *      \w & ^\S = nothing
             * Unfortunately, some vendor locales do not meet the Posix
             * standard, in particular almost everything by Microsoft.
             * The loop below just changes e.g., \w into \W and vice versa */

            regnode_charclass_posixl temp;
            int add = 1;    /* To calculate the index of the complement */

            ANYOF_POSIXL_ZERO(&temp);
            for (i = 0; i < ANYOF_MAX; i++) {
                assert(i % 2 != 0
                       || ! ANYOF_POSIXL_TEST((regnode_charclass_posixl*) and_with, i)
                       || ! ANYOF_POSIXL_TEST((regnode_charclass_posixl*) and_with, i + 1));

                if (ANYOF_POSIXL_TEST((regnode_charclass_posixl*) and_with, i)) {
                    ANYOF_POSIXL_SET(&temp, i + add);
                }
                add = 0 - add; /* 1 goes to -1; -1 goes to 1 */
            }
            ANYOF_POSIXL_AND(&temp, ssc);

        } /* else ssc already has no posixes */
    } /* else: Not inverted.  This routine is a no-op if 'and_with' is an SSC
         in its initial state */
    else if (! is_ANYOF_SYNTHETIC(and_with)
             || ! ssc_is_cp_posixl_init(pRExC_state, (regnode_ssc *)and_with))
    {
        /* But if 'ssc' is in its initial state, the result is just 'and_with';
         * copy it over 'ssc' */
        if (ssc_is_cp_posixl_init(pRExC_state, ssc)) {
            if (is_ANYOF_SYNTHETIC(and_with)) {
                StructCopy(and_with, ssc, regnode_ssc);
            }
            else {
                ssc->invlist = anded_cp_list;
                ANYOF_POSIXL_ZERO(ssc);
                if (ANYOF_FLAGS(and_with) & ANYOF_MATCHES_POSIXL) {
                    ANYOF_POSIXL_OR((regnode_charclass_posixl*) and_with, ssc);
                }
            }
        }
        else if (ANYOF_POSIXL_SSC_TEST_ANY_SET(ssc)
                 || (ANYOF_FLAGS(and_with) & ANYOF_MATCHES_POSIXL))
        {
            /* One or the other of P1, P2 is non-empty. */
            if (ANYOF_FLAGS(and_with) & ANYOF_MATCHES_POSIXL) {
                ANYOF_POSIXL_AND((regnode_charclass_posixl*) and_with, ssc);
            }
            ssc_union(ssc, anded_cp_list, FALSE);
        }
        else { /* P1 = P2 = empty */
            ssc_intersection(ssc, anded_cp_list, FALSE);
        }
    }
}