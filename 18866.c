term_cursor_shape(int shape, int blink)
{
    if (*T_CSH != NUL)
    {
	OUT_STR(tgoto((char *)T_CSH, 0, shape * 2 - blink));
	out_flush();
    }
    else
    {
	int do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	if (blink_state_is_inverted())
	    do_blink = !blink;

	if (do_blink && *T_VS != NUL)
	{
	    out_str(T_VS);
	    out_flush();
	}
	else if (!do_blink && *T_CVS != NUL)
	{
	    out_str(T_CVS);
	    out_flush();
	}
    }
}