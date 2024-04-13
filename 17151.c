mkdir_recurse(char_u *dir, int prot)
{
    char_u	*p;
    char_u	*updir;
    int		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    if (p <= get_past_head(dir))
	return OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    if (updir == NULL)
	return FAIL;
    if (mch_isdir(updir))
	r = OK;
    else if (mkdir_recurse(updir, prot) == OK)
	r = vim_mkdir_emsg(updir, prot);
    vim_free(updir);
    return r;
}