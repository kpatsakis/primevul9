S_is_ssc_worth_it(const RExC_state_t * pRExC_state, const regnode_ssc * ssc)
{
    /* The synthetic start class is used to hopefully quickly winnow down
     * places where a pattern could start a match in the target string.  If it
     * doesn't really narrow things down that much, there isn't much point to
     * having the overhead of using it.  This function uses some very crude
     * heuristics to decide if to use the ssc or not.
     *
     * It returns TRUE if 'ssc' rules out more than half what it considers to
     * be the "likely" possible matches, but of course it doesn't know what the
     * actual things being matched are going to be; these are only guesses
     *
     * For /l matches, it assumes that the only likely matches are going to be
     *      in the 0-255 range, uniformly distributed, so half of that is 127
     * For /a and /d matches, it assumes that the likely matches will be just
     *      the ASCII range, so half of that is 63
     * For /u and there isn't anything matching above the Latin1 range, it
     *      assumes that that is the only range likely to be matched, and uses
     *      half that as the cut-off: 127.  If anything matches above Latin1,
     *      it assumes that all of Unicode could match (uniformly), except for
     *      non-Unicode code points and things in the General Category "Other"
     *      (unassigned, private use, surrogates, controls and formats).  This
     *      is a much large number. */

    U32 count = 0;      /* Running total of number of code points matched by
                           'ssc' */
    UV start, end;      /* Start and end points of current range in inversion
                           list */
    const U32 max_code_points = (LOC)
                                ?  256
                                : ((   ! UNI_SEMANTICS
                                     || invlist_highest(ssc->invlist) < 256)
                                  ? 128
                                  : NON_OTHER_COUNT);
    const U32 max_match = max_code_points / 2;

    PERL_ARGS_ASSERT_IS_SSC_WORTH_IT;

    invlist_iterinit(ssc->invlist);
    while (invlist_iternext(ssc->invlist, &start, &end)) {
        if (start >= max_code_points) {
            break;
        }
        end = MIN(end, max_code_points - 1);
        count += end - start + 1;
        if (count >= max_match) {
            invlist_iterfinish(ssc->invlist);
            return FALSE;
        }
    }

    return TRUE;
}