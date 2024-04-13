ExpandOldSetting(int *num_file, char_u ***file)
{
    char_u  *var = NULL;	/* init for GCC */
    char_u  *buf;

    *num_file = 0;
    *file = (char_u **)alloc((unsigned)sizeof(char_u *));
    if (*file == NULL)
	return FAIL;

    /*
     * For a terminal key code expand_option_idx is < 0.
     */
    if (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	if (var == NULL)
	    expand_option_idx = findoption(expand_option_name);
    }

    if (expand_option_idx >= 0)
    {
	/* put string of option value in NameBuff */
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    else if (var == NULL)
	var = (char_u *)"";

    /* A backslash is required before some characters.  This is the reverse of
     * what happens in do_set(). */
    buf = vim_strsave_escaped(var, escape_chars);

    if (buf == NULL)
    {
	vim_free(*file);
	*file = NULL;
	return FAIL;
    }

#ifdef BACKSLASH_IN_FILENAME
    /* For MS-Windows et al. we don't double backslashes at the start and
     * before a file name character. */
    for (var = buf; *var != NUL; mb_ptr_adv(var))
	if (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif

    *file[0] = buf;
    *num_file = 1;
    return OK;
}