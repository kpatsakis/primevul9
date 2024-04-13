vim_regcomp(char_u *expr_arg, int re_flags)
{
    regprog_T   *prog = NULL;
    char_u	*expr = expr_arg;
    int		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    if (STRNCMP(expr, "\\%#=", 4) == 0)
    {
	int newengine = expr[4] - '0';

	if (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = expr[4] - '0';
	    expr += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	else
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.expr = expr;
    nfa_regengine.expr = expr;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    if (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(expr,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    else
	prog = bt_regengine.regcomp(expr, re_flags);

    // Check for error compiling regexp with initial engine.
    if (prog == NULL)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	if (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    if (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", expr);
		fclose(f);
	    }
	    else
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#endif
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	if (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(expr);
#endif
	    prog = bt_regengine.regcomp(expr, re_flags);
	}
    }

    if (prog != NULL)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    return prog;
}