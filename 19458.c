get_pseudo_mouse_code(
    int	    button,	// eg MOUSE_LEFT
    int	    is_click,
    int	    is_drag)
{
    int	    i;

    for (i = 0; mouse_table[i].pseudo_code; i++)
	if (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    if (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		if (mouse_col < 0)
		    mouse_col = 0;
		else
		    mouse_col -= MOUSE_COLOFF;
		if (mouse_table[i].pseudo_code == (int)KE_LEFTMOUSE)
		    return (int)KE_LEFTMOUSE_NM;
		if (mouse_table[i].pseudo_code == (int)KE_LEFTRELEASE)
		    return (int)KE_LEFTRELEASE_NM;
	    }
#endif
	    return mouse_table[i].pseudo_code;
	}
    return (int)KE_IGNORE;	    // not recognized, ignore it
}