option_value2string(
    struct vimoption	*opp,
    int			opt_flags)	/* OPT_GLOBAL and/or OPT_LOCAL */
{
    char_u	*varp;

    varp = get_varp_scope(opp, opt_flags);

    if (opp->flags & P_NUM)
    {
	long wc = 0;

	if (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_key_name((int)wc, 0));
	else if (wc != 0)
	    STRCPY(NameBuff, transchar((int)wc));
	else
	    sprintf((char *)NameBuff, "%ld", *(long *)varp);
    }
    else    /* P_STRING */
    {
	varp = *(char_u **)(varp);
	if (varp == NULL)		    /* just in case */
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	/* don't show the actual value of 'key', only that it's set */
	else if (opp->var == (char_u *)&p_key && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	else if (opp->flags & P_EXPAND)
	    home_replace(NULL, varp, NameBuff, MAXPATHL, FALSE);
	/* Translate 'pastetoggle' into special key names */
	else if ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	else
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}