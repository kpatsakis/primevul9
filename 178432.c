report_re_switch(char_u *pat)
{
    if (p_verbose > 0)
    {
	verbose_enter();
	msg_puts(_("Switching to backtracking RE engine for pattern: "));
	msg_puts((char *)pat);
	verbose_leave();
    }
}