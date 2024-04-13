scroll_start(void)
{
    if (*T_VS != NUL && *T_CVS != NUL)
    {
	MAY_WANT_TO_LOG_THIS;
	out_str(T_VS);
	out_str(T_CVS);
	screen_start();		// don't know where cursor is now
    }
}