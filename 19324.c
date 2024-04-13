redrawcmd(void)
{
    int save_in_echowindow = in_echowindow;

    if (cmd_silent)
	return;

    // when 'incsearch' is set there may be no command line while redrawing
    if (ccline.cmdbuff == NULL)
    {
	windgoto(cmdline_row, 0);
	msg_clr_eos();
	return;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    msg_start();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    msg_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    msg_clr_eos();
    msg_no_more = FALSE;

    set_cmdspos_cursor();
    if (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    msg_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}