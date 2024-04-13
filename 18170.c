updateWindow(win_T *wp)
{
    // return if already busy updating
    if (updating_screen)
	return;

    update_prepare();

#ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    if (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    if (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
#endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    if (redraw_tabline)
	draw_tabline();

    if (wp->w_redr_status
# ifdef FEAT_CMDL_INFO
	    || p_ru
# endif
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_status(wp, FALSE);

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
#endif

    update_finish();
}