get_text_locked_msg(void)
{
#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0)
	return e_invalid_in_cmdline_window;
#endif
    if (textwinlock != 0)
	return e_not_allowed_to_change_text_or_change_window;
    return e_not_allowed_to_change_text_here;
}