S_pat_upgrade_to_utf8(pTHX_ RExC_state_t * const pRExC_state,
		    char **pat_p, STRLEN *plen_p, int num_code_blocks)
{
    U8 *const src = (U8*)*pat_p;
    U8 *dst, *d;
    int n=0;
    STRLEN s = 0;
    bool do_end = 0;
    GET_RE_DEBUG_FLAGS_DECL;

    DEBUG_PARSE_r(Perl_re_printf( aTHX_
        "UTF8 mismatch! Converting to utf8 for resizing and compile\n"));

    Newx(dst, *plen_p * 2 + 1, U8);
    d = dst;

    while (s < *plen_p) {
        append_utf8_from_native_byte(src[s], &d);

        if (n < num_code_blocks) {
            assert(pRExC_state->code_blocks);
            if (!do_end && pRExC_state->code_blocks->cb[n].start == s) {
                pRExC_state->code_blocks->cb[n].start = d - dst - 1;
                assert(*(d - 1) == '(');
                do_end = 1;
            }
            else if (do_end && pRExC_state->code_blocks->cb[n].end == s) {
                pRExC_state->code_blocks->cb[n].end = d - dst - 1;
                assert(*(d - 1) == ')');
                do_end = 0;
                n++;
            }
        }
        s++;
    }
    *d = '\0';
    *plen_p = d - dst;
    *pat_p = (char*) dst;
    SAVEFREEPV(*pat_p);
    RExC_orig_utf8 = RExC_utf8 = 1;
}