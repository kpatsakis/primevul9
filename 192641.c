S_setup_longest(pTHX_ RExC_state_t *pRExC_state,
                      struct reg_substr_datum  *rsd,
                      struct scan_data_substrs *sub,
                      STRLEN longest_length)
{
    /* This is the common code for setting up the floating and fixed length
     * string data extracted from Perl_re_op_compile() below.  Returns a boolean
     * as to whether succeeded or not */

    I32 t;
    SSize_t ml;
    bool eol  = cBOOL(sub->flags & SF_BEFORE_EOL);
    bool meol = cBOOL(sub->flags & SF_BEFORE_MEOL);

    if (! (longest_length
           || (eol /* Can't have SEOL and MULTI */
               && (! meol || (RExC_flags & RXf_PMf_MULTILINE)))
          )
            /* See comments for join_exact for why REG_UNFOLDED_MULTI_SEEN */
        || (RExC_seen & REG_UNFOLDED_MULTI_SEEN))
    {
        return FALSE;
    }

    /* copy the information about the longest from the reg_scan_data
        over to the program. */
    if (SvUTF8(sub->str)) {
        rsd->substr      = NULL;
        rsd->utf8_substr = sub->str;
    } else {
        rsd->substr      = sub->str;
        rsd->utf8_substr = NULL;
    }
    /* end_shift is how many chars that must be matched that
        follow this item. We calculate it ahead of time as once the
        lookbehind offset is added in we lose the ability to correctly
        calculate it.*/
    ml = sub->minlenp ? *(sub->minlenp) : (SSize_t)longest_length;
    rsd->end_shift = ml - sub->min_offset
        - longest_length
            /* XXX SvTAIL is always false here - did you mean FBMcf_TAIL
             * intead? - DAPM
            + (SvTAIL(sub->str) != 0)
            */
        + sub->lookbehind;

    t = (eol/* Can't have SEOL and MULTI */
         && (! meol || (RExC_flags & RXf_PMf_MULTILINE)));
    fbm_compile(sub->str, t ? FBMcf_TAIL : 0);

    return TRUE;
}