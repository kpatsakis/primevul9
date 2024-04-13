set_string_default(char *name, char_u *val)
{
    char_u	*p;
    int		opt_idx;

    p = vim_strsave(val);
    if (p != NULL)		/* we don't want a NULL */
    {
	opt_idx = findoption((char_u *)name);
	if (opt_idx >= 0)
	{
	    if (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}