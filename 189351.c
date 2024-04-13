set_string_option(
    int		opt_idx,
    char_u	*value,
    int		opt_flags)	/* OPT_LOCAL and/or OPT_GLOBAL */
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#if defined(FEAT_AUTOCMD) && defined(FEAT_EVAL)
    char_u	*saved_oldval = NULL;
#endif
    char_u	*r = NULL;

    if (options[opt_idx].var == NULL)	/* don't set hidden option */
	return NULL;

    s = vim_strsave(value);
    if (s != NULL)
    {
	varp = (char_u **)get_varp_scope(&(options[opt_idx]),
		(opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		    ? (((int)options[opt_idx].indir & PV_BOTH)
			? OPT_GLOBAL : OPT_LOCAL)
		    : opt_flags);
	oldval = *varp;
	*varp = s;

#if defined(FEAT_AUTOCMD) && defined(FEAT_EVAL)
	if (!starting
# ifdef FEAT_CRYPT
		&& options[opt_idx].indir != PV_KEY
# endif
		)
	    saved_oldval = vim_strsave(oldval);
#endif
	if ((r = did_set_string_option(opt_idx, varp, TRUE, oldval, NULL,
							   opt_flags)) == NULL)
	    did_set_option(opt_idx, opt_flags, TRUE);

	/* call autocomamnd after handling side effects */
#if defined(FEAT_AUTOCMD) && defined(FEAT_EVAL)
	if (saved_oldval != NULL)
	{
	    char_u buf_type[7];
	    sprintf((char *)buf_type, "%s",
		(opt_flags & OPT_LOCAL) ? "local" : "global");
	    set_vim_var_string(VV_OPTION_NEW, *varp, -1);
	    set_vim_var_string(VV_OPTION_OLD, saved_oldval, -1);
	    set_vim_var_string(VV_OPTION_TYPE, buf_type, -1);
	    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname, NULL, FALSE, NULL);
	    reset_v_option_vars();
	    vim_free(saved_oldval);
	}
#endif
    }
    return r;
}