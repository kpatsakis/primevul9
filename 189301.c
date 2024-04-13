set_init_2(void)
{
    int		idx;

    /*
     * 'scroll' defaults to half the window height. Note that this default is
     * wrong when the window height changes.
     */
    set_number_default("scroll", (long)((long_u)Rows >> 1));
    idx = findoption((char_u *)"scroll");
    if (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    /*
     * 'window' is only for backwards compatibility with Vi.
     * Default is Rows - 1.
     */
    if (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_number_default("window", Rows - 1);

    /* For DOS console the default is always black. */
#if !((defined(WIN3264)) && !defined(FEAT_GUI))
    /*
     * If 'background' wasn't set by the user, try guessing the value,
     * depending on the terminal name.  Only need to check for terminals
     * with a dark background, that can handle color.
     */
    idx = findoption((char_u *)"bg");
    if (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(NULL, idx, (char_u *)"dark", OPT_FREE, 0);
	/* don't mark it as set, when starting the GUI it may be
	 * changed again */
	options[idx].flags &= ~P_WAS_SET;
    }
#endif

#ifdef CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR); /* set cursor shapes from 'guicursor' */
#endif
#ifdef FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);  /* set mouse shapes from 'mouseshape' */
#endif
#ifdef FEAT_PRINTER
    (void)parse_printoptions();	    /* parse 'printoptions' default value */
#endif
}