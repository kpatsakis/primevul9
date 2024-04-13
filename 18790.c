set_color_count(int nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    if (t_colors > 1)
	sprintf((char *)nr_colors, "%d", t_colors);
    else
	*nr_colors = NUL;
    set_string_option_direct((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}