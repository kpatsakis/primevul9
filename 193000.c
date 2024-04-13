spell_find_suggest(
    char_u	*badptr,
    int		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    int		maxcount,
    int		banbadword,	// don't include badword in suggestions
    int		need_cap,	// word should start with capital
    int		interactive)
{
    hlf_T	attr = HLF_COUNT;
    char_u	buf[MAXPATHL];
    char_u	*p;
    int		do_combine = FALSE;
    char_u	*sps_copy;
#ifdef FEAT_EVAL
    static int	expr_busy = FALSE;
#endif
    int		c;
    int		i;
    langp_T	*lp;
    int		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_POINTER(su);
    ga_init2(&su->su_ga, (int)sizeof(suggest_T), 10);
    ga_init2(&su->su_sga, (int)sizeof(suggest_T), 10);
    if (*badptr == NUL)
	return;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    if (badlen != 0)
	su->su_badlen = badlen;
    else
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, NULL, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    if (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (void)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    if (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    for (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ENTRY(curbuf->b_s.b_langp, i);
	if (lp->lp_sallang != NULL)
	{
	    su->su_sallang = lp->lp_sallang;
	    break;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    if (su->su_sallang != NULL)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    if (!SPELL_ISUPPER(c) && attr == HLF_COUNT)
    {
	make_case_word(su->su_badword, buf, WF_ONECAP);
	add_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    if (banbadword)
	add_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    if (sps_copy == NULL)
	return;

    // Loop over the items in 'spellsuggest'.
    for (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	if (STRNCMP(buf, "expr:", 5) == 0)
	{
#ifdef FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    if (!expr_busy)
	    {
		expr_busy = TRUE;
		spell_suggest_expr(su, buf + 5);
		expr_busy = FALSE;
	    }
#endif
	}
	else if (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	else if (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    if (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    if (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}