redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    int		item_idx = 0;
    int		item_count = 0;
    int		col = 0;
    int		next_col;
    int		off = (int)(current_ScreenLine - ScreenLines);
    int		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    int		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_items);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_count;
    wp->w_winbar_items = ALLOC_CLEAR_MULT(winbar_item_T, item_count + 1);

    // TODO: use fewer spaces if there is not enough room
    for (menu = wp->w_winbar->children;
			  menu != NULL && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	if (++col >= wp->w_width)
	    break;
	if (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    if (++col >= wp->w_width)
		break;
	}

	wp->w_winbar_items[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	if (++col >= wp->w_width)
	    break;

	next_col = text_to_screenline(wp, menu->name, col);
	while (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_items[item_idx].wb_endcol = col;
	wp->w_winbar_items[item_idx].wb_menu = menu;
	++item_idx;

	if (col >= wp->w_width)
	    break;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    while (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_items[item_idx].wb_menu = NULL; // end marker

    screen_line(wp->w_winrow, wp->w_wincol, (int)wp->w_width,
							  (int)wp->w_width, 0);
}