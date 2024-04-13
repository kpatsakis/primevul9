scroll_region_set(win_T *wp, int off)
{
    OUT_STR(tgoto((char *)T_CS, W_WINROW(wp) + wp->w_height - 1,
							 W_WINROW(wp) + off));
    if (*T_CSV != NUL && wp->w_width != Columns)
	OUT_STR(tgoto((char *)T_CSV, wp->w_wincol + wp->w_width - 1,
							       wp->w_wincol));
    screen_start();		    // don't know where cursor is now
}