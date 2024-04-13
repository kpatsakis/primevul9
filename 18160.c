redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    if (wp->w_redraw_top == 0 || wp->w_redraw_top > lnum)
	wp->w_redraw_top = lnum;
    if (wp->w_redraw_bot == 0 || wp->w_redraw_bot < lnum)
	wp->w_redraw_bot = lnum;
    redraw_win_later(wp, VALID);
}