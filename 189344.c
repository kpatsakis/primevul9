free_all_options(void)
{
    int		i;

    for (i = 0; !istermoption(&options[i]); i++)
    {
	if (options[i].indir == PV_NONE)
	{
	    /* global option: free value and default value. */
	    if (options[i].flags & P_ALLOCED && options[i].var != NULL)
		free_string_option(*(char_u **)options[i].var);
	    if (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	else if (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    /* buffer-local option: free global value */
	    free_string_option(*(char_u **)options[i].var);
    }
}