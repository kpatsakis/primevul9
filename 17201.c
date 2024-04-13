modify_fname(
    char_u	*src,		// string with modifiers
    int		tilde_file,	// "~" is a file name, not $HOME
    int		*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    int		*fnamelen)	// length of fnamep
{
    int		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    int		c;
    int		has_fullname = 0;
    int		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    int		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    if (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	if ((*fnamep)[0] == '~'
#if !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_IN_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#endif
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    if (*fnamep == NULL)
		return -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	for (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    if (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		break;
	}

	// FullName_save() is slow, don't use it when not needed.
	if (*p != NUL || !vim_isAbsName(*fnamep))
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    if (*fnamep == NULL)
		return -1;
	}

#ifdef MSWIN
# if _WIN32_WINNT >= 0x0500
	if (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    if (wfname != NULL)
	    {
		if (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    if (q != NULL)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#endif
	// Append a path separator to a directory.
	if (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    if (*fnamep == NULL)
		return -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    while (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	if (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	if (!has_fullname && !has_homerelative)
	{
	    if (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    else
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	else
	    p = *fnamep;

	has_fullname = 0;

	if (p != NULL)
	{
	    if (c == '.')
	    {
		size_t	namelen;

		mch_dirname(dirname, MAXPATHL);
		if (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    if (s != NULL)
		    {
			home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}
		namelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		if (fnamencmp(p, dirname, namelen) == 0)
		{
		    p += namelen;
		    if (vim_ispathsep(*p))
		    {
			while (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			if (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    else
	    {
		home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		if (*dirname == '~')
		{
		    s = vim_strsave(dirname);
		    if (s != NULL)
		    {
			*fnamep = s;
			vim_free(*bufp);
			*bufp = s;
			has_homerelative = TRUE;
		    }
		}
	    }
	    vim_free(pbuf);
	}
    }

    tail = gettail(*fnamep);
    *fnamelen = (int)STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    while (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	while (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = (int)(tail - *fnamep);
#ifdef VMS
	if (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	if (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    if (p == NULL)
		return -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	else
	{
	    while (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    if (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#ifdef MSWIN
	has_shortname = 1;
#endif
    }

#ifdef MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    if (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	if (*fnamelen < (int)STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    if (p == NULL)
		return -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	if (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    if (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		return -1;
	}
	else
	{
	    int		l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    if (get_short_pathname(fnamep, bufp, &l) == FAIL)
		return -1;

	    if (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		if (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    return -1;
	    }
	    *fnamelen = l;
	}
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    if (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= (int)(tail - *fnamep);
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    while (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	if (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	else
	    s = *fnamep + *fnamelen - 1;
	for ( ; s > tail; --s)
	    if (s[0] == '.')
		break;
	if (src[*usedlen + 1] == 'e')		// :e
	{
	    if (s > tail)
	    {
		*fnamelen += (int)(*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		for ( ; s > *fnamep; --s)
		    if (s[0] == ';')
			break;
		if (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    else if (*fnamep <= tail)
		*fnamelen = 0;
	}
	else				// :r
	{
	    char_u *limit = *fnamep;

	    if (limit < tail)
		limit = tail;
	    if (s > limit)	// remove one extension
		*fnamelen = (int)(s - *fnamep);
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    if (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	int	    sep;
	char_u	    *flags;
	int	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	if (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	if (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    if (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		if (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    if (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			if (sub != NULL && str != NULL)
			{
			    *usedlen = (int)(p + 1 - src);
			    s = do_string_sub(str, pat, sub, NULL, flags);
			    if (s != NULL)
			    {
				*fnamep = s;
				*fnamelen = (int)STRLEN(s);
				vim_free(*bufp);
				*bufp = s;
				didit = TRUE;
			    }
			}
			vim_free(sub);
			vim_free(str);
		    }
		    vim_free(pat);
		}
	    }
	    // after using ":s", repeat all the modifiers
	    if (didit)
		goto repeat;
	}
    }

    if (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	if (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	if (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	if (p == NULL)
	    return -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = (int)STRLEN(p);
	*usedlen += 2;
    }

    return valid;
}