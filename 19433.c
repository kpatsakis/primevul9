f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    int		row = mouse_row;
    int		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;

    if (rettv_dict_alloc(rettv) == FAIL)
	return;
    d = rettv->vval.v_dict;

    dict_add_number(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_number(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    if (wp != NULL)
    {
	int	top_off = 0;
	int	left_off = 0;
	int	height = wp->w_height + wp->w_status_height;

#ifdef FEAT_PROP_POPUP
	if (WIN_IS_POPUP(wp))
	{
	    top_off = popup_top_extra(wp);
	    left_off = popup_left_extra(wp);
	    height = popup_height(wp);
	}
#endif
	if (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= top_off;
	    col -= left_off;
	    if (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(void)mouse_comp_pos(wp, &row, &col, &lnum, NULL);
		col = vcol2col(wp, lnum, col);
		column = col + 1;
	    }
	}
    }
    dict_add_number(d, "winid", winid);
    dict_add_number(d, "winrow", winrow);
    dict_add_number(d, "wincol", wincol);
    dict_add_number(d, "line", (varnumber_T)lnum);
    dict_add_number(d, "column", column);
}