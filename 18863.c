term_cursor_mode(int forced)
{
    static int showing_mode = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    if (!full_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	if (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    term_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	return;
    }

    if ((State & MODE_REPLACE) == MODE_REPLACE)
    {
	if (forced || showing_mode != MODE_REPLACE)
	{
	    if (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    else
		p = T_CSI;	// fall back to Insert mode cursor
	    if (*p != NUL)
	    {
		out_str(p);
		showing_mode = MODE_REPLACE;
	    }
	}
    }
    else if (State & MODE_INSERT)
    {
	if ((forced || showing_mode != MODE_INSERT) && *T_CSI != NUL)
	{
	    out_str(T_CSI);	    // Insert mode cursor
	    showing_mode = MODE_INSERT;
	}
    }
    else if (forced || showing_mode != MODE_NORMAL)
    {
	out_str(T_CEI);		    // non-Insert mode cursor
	showing_mode = MODE_NORMAL;
    }
}