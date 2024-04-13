get_option_value(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    /* NULL when only checking existence */
    int		opt_flags)
{
    int		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    if (opt_idx < 0)		    /* unknown option */
	return -3;

    varp = get_varp_scope(&(options[opt_idx]), opt_flags);

    if (options[opt_idx].flags & P_STRING)
    {
	if (varp == NULL)		    /* hidden option */
	    return -2;
	if (stringval != NULL)
	{
#ifdef FEAT_CRYPT
	    /* never return the value of the crypt key */
	    if ((char_u **)varp == &curbuf->b_p_key
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strsave((char_u *)"*****");
	    else
#endif
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	return 0;
    }

    if (varp == NULL)		    /* hidden option */
	return -1;
    if (options[opt_idx].flags & P_NUM)
	*numval = *(long *)varp;
    else
    {
	/* Special case: 'modified' is b_changed, but we also want to consider
	 * it set when 'ff' or 'fenc' changed. */
	if ((int *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	else
	    *numval = (long) *(int *)varp;
    }
    return 1;
}