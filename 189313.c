set_string_option_direct(
    char_u	*name,
    int		opt_idx,
    char_u	*val,
    int		opt_flags,	/* OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL */
    int		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    int		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    int		idx = opt_idx;

    if (idx == -1)		/* use name */
    {
	idx = findoption(name);
	if (idx < 0)	/* not found (should not happen) */
	{
	    EMSG2(_(e_intern2), "set_string_option_direct()");
	    EMSG2(_("For option %s"), name);
	    return;
	}
    }

    if (options[idx].var == NULL)	/* can't set hidden option */
	return;

    s = vim_strsave(val);
    if (s != NULL)
    {
	varp = (char_u **)get_varp_scope(&(options[idx]),
					       both ? OPT_LOCAL : opt_flags);
	if ((opt_flags & OPT_FREE) && (options[idx].flags & P_ALLOCED))
	    free_string_option(*varp);
	*varp = s;

	/* For buffer/window local option may also set the global value. */
	if (both)
	    set_string_option_global(idx, varp);

	options[idx].flags |= P_ALLOCED;

	/* When setting both values of a global option with a local value,
	 * make the local value empty, so that the global value is used. */
	if (((int)options[idx].indir & PV_BOTH) && both)
	{
	    free_string_option(*varp);
	    *varp = empty_option;
	}
# ifdef FEAT_EVAL
	if (set_sid != SID_NONE)
	    set_option_scriptID_idx(idx, opt_flags,
					set_sid == 0 ? current_SID : set_sid);
# endif
    }
}