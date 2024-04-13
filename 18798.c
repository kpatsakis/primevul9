switch_to_8bit(void)
{
    int		i;
    int		c;

    // Only need to do something when not already using 8-bit codes.
    if (!term_is_8bit(T_NAME))
    {
	for (i = 0; i < tc_len; ++i)
	{
	    c = term_7to8bit(termcodes[i].code);
	    if (c != 0)
	    {
		STRMOVE(termcodes[i].code + 1, termcodes[i].code + 2);
		termcodes[i].code[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR(("Switching to 8 bit"));
}