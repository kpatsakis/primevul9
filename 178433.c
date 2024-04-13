vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*line,  // string to match against
    colnr_T	col,    // column to start looking for match
    int		nl)
{
    int		result;
    regexec_T	rex_save;
    int		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    if (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	return FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    if (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = NULL;
    rex.reg_endp = NULL;
    rex.reg_startpos = NULL;
    rex.reg_endpos = NULL;

    result = rmp->regprog->engine->regexec_nl(rmp, line, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    if (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	int    save_p_re = p_re;
	int    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	vim_regfree(rmp->regprog);
	if (pat != NULL)
	{
#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    if (rmp->regprog != NULL)
	    {
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, line, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    if (rex_in_use)
	rex = rex_save;

    return result > 0;
}