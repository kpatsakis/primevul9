setmouse(void)
{
    int	    checkfor;

# ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
# endif

    // Should be outside proc, but may break MOUSESHAPE
#  ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    if (gui.in_use)
	return;
#  endif
    // be quick when mouse is off
    if (*p_mouse == NUL || has_mouse_termcode == 0)
	return;

    // don't switch mouse on when not in raw mode (Ex mode)
    if (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	return;
    }

    if (VIsual_active)
	checkfor = MOUSE_VISUAL;
    else if (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    else if (State & MODE_INSERT)
	checkfor = MOUSE_INSERT;
    else if (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    else if (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    else
	checkfor = MOUSE_NORMAL;    // assume normal mode

    if (mouse_has(checkfor))
	mch_setmouse(TRUE);
    else
	mch_setmouse(FALSE);
}