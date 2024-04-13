ins_start_select(int c)
{
    if (!km_startsel)
	return FALSE;
    switch (c)
    {
	case K_KHOME:
	case K_KEND:
	case K_PAGEUP:
	case K_KPAGEUP:
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
# ifdef MACOS_X
	case K_LEFT:
	case K_RIGHT:
	case K_UP:
	case K_DOWN:
	case K_END:
	case K_HOME:
# endif
	    if (!(mod_mask & MOD_MASK_SHIFT))
		break;
	    // FALLTHROUGH
	case K_S_LEFT:
	case K_S_RIGHT:
	case K_S_UP:
	case K_S_DOWN:
	case K_S_END:
	case K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    if (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuff(buf);
	    }
	    stuffcharReadbuff(c);
	    return TRUE;
    }
    return FALSE;
}