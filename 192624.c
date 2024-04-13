S_has_runtime_code(pTHX_ RExC_state_t * const pRExC_state,
		    char *pat, STRLEN plen)
{
    int n = 0;
    STRLEN s;
    
    PERL_UNUSED_CONTEXT;

    for (s = 0; s < plen; s++) {
	if (   pRExC_state->code_blocks
            && n < pRExC_state->code_blocks->count
	    && s == pRExC_state->code_blocks->cb[n].start)
	{
	    s = pRExC_state->code_blocks->cb[n].end;
	    n++;
	    continue;
	}
	/* TODO ideally should handle [..], (#..), /#.../x to reduce false
	 * positives here */
	if (pat[s] == '(' && s+2 <= plen && pat[s+1] == '?' &&
	    (pat[s+2] == '{'
                || (s + 2 <= plen && pat[s+2] == '?' && pat[s+3] == '{'))
	)
	    return 1;
    }
    return 0;
}