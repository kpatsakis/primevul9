put_setstring(
    FILE	*fd,
    char	*cmd,
    char	*name,
    char_u	**valuep,
    int		expand)
{
    char_u	*s;
    char_u	*buf;

    if (fprintf(fd, "%s %s=", cmd, name) < 0)
	return FAIL;
    if (*valuep != NULL)
    {
	/* Output 'pastetoggle' as key names.  For other
	 * options some characters have to be escaped with
	 * CTRL-V or backslash */
	if (valuep == &p_pt)
	{
	    s = *valuep;
	    while (*s != NUL)
		if (put_escstr(fd, str2special(&s, FALSE), 2) == FAIL)
		    return FAIL;
	}
	else if (expand)
	{
	    buf = alloc(MAXPATHL);
	    if (buf == NULL)
		return FAIL;
	    home_replace(NULL, *valuep, buf, MAXPATHL, FALSE);
	    if (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		return FAIL;
	    }
	    vim_free(buf);
	}
	else if (put_escstr(fd, *valuep, 2) == FAIL)
	    return FAIL;
    }
    if (put_eol(fd) < 0)
	return FAIL;
    return OK;
}