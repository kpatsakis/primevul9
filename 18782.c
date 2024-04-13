report_term_error(char *error_msg, char_u *term)
{
    struct builtin_term *termp;
    int			i;

    mch_errmsg("\r\n");
    if (error_msg != NULL)
    {
	mch_errmsg(error_msg);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((char *)term);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");
    for (termp = &(builtin_termcaps[0]); termp->bt_string != NULL; ++termp)
    {
	if (termp->bt_entry == (int)KS_NAME
		&& STRCMP(termp->bt_string, "gui") != 0)
	{
#ifdef HAVE_TGETENT
	    mch_errmsg("    builtin_");
#else
	    mch_errmsg("    ");
#endif
	    mch_errmsg(termp->bt_string);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    for (i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}