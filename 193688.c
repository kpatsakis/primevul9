ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    int		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    if (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    if (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    else
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, NULL,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	if (dirname == NULL || !mch_isdir(dirname))
	    semsg(_("E150: Not a directory: %s"), eap->arg);
	else
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}