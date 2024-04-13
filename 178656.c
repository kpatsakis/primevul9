char *theme_format_expand_data(THEME_REC *theme, const char **format, theme_rm_col default_fg,
                               theme_rm_col default_bg, theme_rm_col *save_last_fg,
                               theme_rm_col *save_last_bg, int flags)
{
	return theme_format_expand_data_rec(theme, format, default_fg, default_bg, save_last_bg,
	                                    save_last_bg, flags, NULL);
}