vimrc_found(char_u *fname, char_u *envname)
{
    int		opt_idx;
    int		dofree = FALSE;
    char_u	*p;

    if (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
	    if (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    if (fname != NULL)
    {
	p = vim_getenv(envname, &dofree);
	if (p == NULL)
	{
	    /* Set $MYVIMRC to the first vimrc file found. */
	    p = FullName_save(fname, FALSE);
	    if (p != NULL)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	}
	else if (dofree)
	    vim_free(p);
    }
}