FullName_save(
    char_u	*fname,
    int		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    if (fname == NULL)
	return NULL;

    buf = alloc(MAXPATHL);
    if (buf != NULL)
    {
	if (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	    new_fname = vim_strsave(buf);
	else
	    new_fname = vim_strsave(fname);
	vim_free(buf);
    }
    return new_fname;
}