dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    int		wildoff,
    int		flags,		// EW_* flags
    int		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    int		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    int		starts_with_dot;
    int		matches;
    int		len;
    int		starstar = FALSE;
    static int	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    int			ok;
    char_u		*p_alt;

    // Expanding "**" may take a long time, check for CTRL-C.
    if (stardepth > 0)
    {
	ui_breakcheck();
	if (got_int)
	    return 0;
    }

    // Make room for file name.  When doing encoding conversion the actual
    // length may be quite a bit longer, thus use the maximum possible length.
    buf = alloc(MAXPATHL);
    if (buf == NULL)
	return 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    while (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	if (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	else if (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    if (e != NULL)
		break;
	    s = p + 1;
	}
	else if (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	if (has_mbyte)
	{
	    len = (*mb_ptr2len)(path_end);
	    STRNCPY(p, path_end, len);
	    p += len;
	    path_end += len;
	}
	else
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    for (p = buf + wildoff; p < s; ++p)
	if (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    for (p = s; p < e; ++p)
	if (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    if (pat == NULL)
    {
	vim_free(buf);
	return 0;
    }

    // compile the regexp into a program
    if (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    if (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    if (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	return 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    if (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	STRCPY(s, path_end + 1);
	++stardepth;
	(void)dos_expandpath(gap, buf, (int)(s - buf), flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    STRCPY(s, "*.*");
    wn = enc_to_utf16(buf, NULL);
    if (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    while (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	if (p == NULL)
	    break;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	if (*wfb.cAlternateFileName == NUL || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	else
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	if ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == NULL
		  || (regmatch.regprog != NULL
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != NULL
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + (s - buf), p, e - s) == 0)))
	{
	    STRCPY(s, p);
	    len = (int)STRLEN(buf);

	    if (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	    {
		// For "**" in the pattern first go deeper in the tree to
		// find matches.
		STRCPY(buf + len, "/**");
		STRCPY(buf + len + 3, path_end);
		++stardepth;
		(void)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		--stardepth;
	    }

	    STRCPY(buf + len, path_end);
	    if (mch_has_exp_wildcard(path_end))
	    {
		// need to expand another component of the path
		// remove backslashes for the remaining components only
		(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
	    }
	    else
	    {
		// no more wildcards, check if there is a match
		// remove backslashes for the remaining components only
		if (*path_end != 0)
		    backslash_halve(buf + len + 1);
		if (mch_getperm(buf) >= 0)	// add existing file
		    addfile(gap, buf, flags);
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = FindNextFileW(hFind, &wfb);
    }

    FindClose(hFind);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - start_len;
    if (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   sizeof(char_u *), pstrcmp);
    return matches;
}