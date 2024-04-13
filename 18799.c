ttest(int pairs)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    if (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    if (*T_CS != NUL)
	scroll_region = TRUE;
    else
	scroll_region = FALSE;

    if (pairs)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	if (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = empty_option;
	if (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = empty_option;
	if (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = empty_option;
	if (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = empty_option;

	// T_VE is needed even though T_VI is not defined
	if (*T_VE == NUL)
	    T_VI = empty_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	if (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	if (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    if (*T_MR == NUL)
		T_SO = T_MD;
	    else
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	if (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    if (*T_MR == NUL)
		T_CZH = T_MD;
	    else
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	if (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = empty_option;
	    T_CSF = empty_option;
	}

	// "AB" and "AF" come in pairs
	if (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = empty_option;
	    T_CAF = empty_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	if (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((char *)T_CCO);
#ifdef FEAT_GUI
    if (!gui.in_use)
#endif
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	if (env_colors != NULL && isdigit(*env_colors))
	{
	    int colors = atoi((char *)env_colors);

	    if (colors != t_colors)
		set_color_count(colors);
	}
    }
}