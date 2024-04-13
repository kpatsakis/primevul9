redraw_all_later(int type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    if (must_redraw < type)
	must_redraw = type;
}