is_mouse_topline(win_T *wp)
{
    return orig_topline == wp->w_topline
#ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
#endif
	;
}