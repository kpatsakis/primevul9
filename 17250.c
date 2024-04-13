f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
#ifdef HAVE_READLINK
    char_u	*buf = NULL;
#endif

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    p = tv_get_string(&argvars[0]);
#ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	if (v != NULL)
	    rettv->vval.v_string = v;
	else
	    rettv->vval.v_string = vim_strsave(p);
    }
#else
# ifdef HAVE_READLINK
    {
	char_u	*cpy;
	int	len;
	char_u	*remain = NULL;
	char_u	*q;
	int	is_relative_to_current = FALSE;
	int	has_trailing_pathsep = FALSE;
	int	limit = 100;

	p = vim_strsave(p);
	if (p == NULL)
	    goto fail;
	if (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	if (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	if (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	if (buf == NULL)
	{
	    vim_free(p);
	    goto fail;
	}

	for (;;)
	{
	    for (;;)
	    {
		len = readlink((char *)p, (char *)buf, MAXPATHL);
		if (len <= 0)
		    break;
		buf[len] = NUL;

		if (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		if (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		if (*q != NUL)
		{
		    if (remain == NULL)
			remain = vim_strsave(q - 1);
		    else
		    {
			cpy = concat_str(q - 1, remain);
			if (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		if (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    q[-1] = NUL;
		    q = gettail(p);
		}
		if (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    if (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		else
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    if (remain == NULL)
		break;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    if (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    if (*q != NUL)
		STRMOVE(remain, q - 1);
	    else
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	if (!vim_ispathsep(*p))
	{
	    if (is_relative_to_current
		    && *p != NUL
		    && !(p[0] == '.'
			&& (p[1] == NUL
			    || vim_ispathsep(p[1])
			    || (p[1] == '.'
				&& (p[2] == NUL
				    || vim_ispathsep(p[2]))))))
	    {
		// Prepend "./".
		cpy = concat_str((char_u *)"./", p);
		if (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    else if (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		while (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		if (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	if (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    if (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
# else
    rettv->vval.v_string = vim_strsave(p);
# endif
#endif

    simplify_filename(rettv->vval.v_string);

#ifdef HAVE_READLINK
fail:
    vim_free(buf);
#endif
    rettv->v_type = VAR_STRING;
}