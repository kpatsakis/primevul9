clear_termoptions(void)
{
    /*
     * Reset a few things before clearing the old options. This may cause
     * outputting a few things that the terminal doesn't understand, but the
     * screen will be cleared later, so this is OK.
     */
#ifdef FEAT_MOUSE_TTY
    mch_setmouse(FALSE);	    /* switch mouse off */
#endif
#ifdef FEAT_TITLE
    mch_restore_title(3);	    /* restore window titles */
#endif
#if defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    /* When starting the GUI close the display opened for the clipboard.
     * After restoring the title, because that will need the display. */
    if (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			/* stop termcap mode */

    free_termoptions();
}