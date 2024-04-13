report_default_term(char_u *term)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((char *)term);
    mch_errmsg("'\r\n");
    if (emsg_silent == 0 && !in_assert_fails)
    {
	screen_start();	// don't know where cursor is now
	out_flush();
	if (!is_not_a_term())
	    ui_delay(2007L, TRUE);
    }
}