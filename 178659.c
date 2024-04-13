static char *theme_replace_expand(THEME_REC *theme, int index,
				  theme_rm_col default_fg, theme_rm_col default_bg,
				  theme_rm_col *last_fg, theme_rm_col *last_bg,
				  char chr, int flags)
{
	GSList *rec;
	char *ret, *abstract, data[2];

	rec = g_slist_nth(theme->replace_values, index);
	g_return_val_if_fail(rec != NULL, NULL);

	data[0] = chr; data[1] = '\0';

	abstract = rec->data;
	abstract = theme_format_expand_data(theme, (const char **) &abstract,
					    default_fg, default_bg,
					    last_fg, last_bg, (flags | EXPAND_FLAG_IGNORE_REPLACES));
	ret = parse_special_string(abstract, NULL, NULL, data, NULL,
				   PARSE_FLAG_ONLY_ARGS);
	g_free(abstract);
	return ret;
}