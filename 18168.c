statusline_row(win_T *wp)
{
#if defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    if (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	return wp->w_winrow;
#endif
    return W_WINROW(wp) + wp->w_height;
}