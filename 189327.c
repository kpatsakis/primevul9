set_options_default(
    int		opt_flags)	/* OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL */
{
    int		i;
#ifdef FEAT_WINDOWS
    win_T	*wp;
    tabpage_T	*tp;
#endif

    for (i = 0; !istermoption(&options[i]); i++)
	if (!(options[i].flags & P_NODEFAULT)
#if defined(FEAT_MBYTE) || defined(FEAT_CRYPT)
		&& (opt_flags == 0
		    || (TRUE
# if defined(FEAT_MBYTE)
			&& options[i].var != (char_u *)&p_enc
# endif
# if defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_key
# endif
			))
#endif
			)
	    set_option_default(i, opt_flags, p_cp);

#ifdef FEAT_WINDOWS
    /* The 'scroll' option must be computed for all windows. */
    FOR_ALL_TAB_WINDOWS(tp, wp)
	win_comp_scroll(wp);
#else
	win_comp_scroll(curwin);
#endif
#ifdef FEAT_CINDENT
    parse_cino(curbuf);
#endif
}