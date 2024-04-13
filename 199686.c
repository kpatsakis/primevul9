input_parse_pane(struct window_pane *wp)
{
	void	*new_data;
	size_t	 new_size;

	new_data = window_pane_get_new_data(wp, &wp->offset, &new_size);
	input_parse_buffer(wp, new_data, new_size);
	window_pane_update_used_data(wp, &wp->offset, new_size);
}