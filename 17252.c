gen_expand_wildcards(
    int		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    int		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    int		flags)		// EW_* flags
{
    int			i;
    garray_T		ga;
    char_u		*p;
    static int		recursive = FALSE;
    int			add_pat;
    int			retval = OK;
#if defined(FEAT_SEARCHPATH)
    int			did_expand_in_path = FALSE;
#endif

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    if (recursive)
#ifdef SPECIAL_WILDCHAR
	return mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#else
	return FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    for (i = 0; i < num_pat; i++)
    {
	if (has_special_wildchar(pat[i])
# ifdef VIM_BACKTICK
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
# endif
	   )
	    return mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, sizeof(char_u *), 30);

    for (i = 0; i < num_pat; ++i)
    {
	add_pat = -1;
	p = pat[i];

#ifdef VIM_BACKTICK
	if (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    if (add_pat == -1)
		retval = FAIL;
	}
	else
#endif
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    if ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE);
		if (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		else if (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    return i;
		}
#endif
	    }

	    /*
	     * If there are wildcards: Expand file names and add each match to
	     * the list.  If there is no match, and EW_NOTFOUND is given, add
	     * the pattern.
	     * If there are no wildcards: Add the file name if it exists or
	     * when EW_NOTFOUND is given.
	     */
	    if (mch_has_exp_wildcard(p))
	    {
#if defined(FEAT_SEARCHPATH)
		if ((flags & EW_PATH)
			&& !mch_isFullName(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    add_pat = expand_in_path(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_in_path = TRUE;
		}
		else
#endif
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	if (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    if (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    else
		addfile(&ga, t, flags);

	    if (t != p)
		vim_free(t);
	}

#if defined(FEAT_SEARCHPATH)
	if (did_expand_in_path && ga.ga_len > 0 && (flags & EW_PATH))
	    uniquefy_paths(&ga, p);
#endif
	if (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    if (retval == FAIL)
	ga_clear(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    return ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
}