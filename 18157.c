win_redraw_last_status(frame_T *frp)
{
    if (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = TRUE;
    else if (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    else // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	while (frp->fr_next != NULL)
	    frp = frp->fr_next;
	win_redraw_last_status(frp);
    }
}