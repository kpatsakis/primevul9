showruler(int always)
{
    if (!always && !redrawing())
	return;
    if (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = TRUE;
	return;
    }
#if defined(FEAT_STL_OPT)
    if ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
	redraw_custom_statusline(curwin);
    else
#endif
#ifdef FEAT_CMDL_INFO
	win_redr_ruler(curwin, always, FALSE);
#endif

#ifdef FEAT_TITLE
    if (need_maketitle
# ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
# endif
       )
	maketitle();
#endif
    // Redraw the tab pages line if needed.
    if (redraw_tabline)
	draw_tabline();
}