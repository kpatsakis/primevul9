set_option_default(
    int		opt_idx,
    int		opt_flags,	/* OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL */
    int		compatible)	/* use Vi default value */
{
    char_u	*varp;		/* pointer to variable for current option */
    int		dvi;		/* index in def_val[] */
    long_u	flags;
    long_u	*flagsp;
    int		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    if (varp != NULL)	    /* skip hidden option, nothing to do for it */
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	if (flags & P_STRING)
	{
	    /* Use set_string_option_direct() for local options to handle
	     * freeing and allocating the value. */
	    if (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(NULL, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    else
	    {
		if ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	else if (flags & P_NUM)
	{
	    if (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    else
	    {
		*(long *)varp = (long)(long_i)options[opt_idx].def_val[dvi];
		/* May also set global value for local option. */
		if (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(long *)varp;
	    }
	}
	else	/* P_BOOL */
	{
	    /* the cast to long is required for Manx C, long_i is needed for
	     * MSVC */
	    *(int *)varp = (int)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    /* 'modeline' defaults to off for root */
	    if (options[opt_idx].indir == PV_ML && getuid() == ROOT_UID)
		*(int *)varp = FALSE;
#endif
	    /* May also set global value for local option. */
	    if (both)
		*(int *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(int *)varp;
	}

	/* The default value is not insecure. */
	flagsp = insecure_flag(opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
    }

#ifdef FEAT_EVAL
    set_option_scriptID_idx(opt_idx, opt_flags, current_SID);
#endif
}