static char *theme_format_expand_data_rec(THEME_REC *theme, const char **format,
                                          theme_rm_col default_fg, theme_rm_col default_bg,
                                          theme_rm_col *save_last_fg, theme_rm_col *save_last_bg,
                                          int flags, GTree *block_list)
{
	GString *str;
	char *ret, *abstract;
	theme_rm_col last_fg, last_bg;
        int recurse_flags;

	last_fg = default_fg;
	last_bg = default_bg;
        recurse_flags = flags & EXPAND_FLAG_RECURSIVE_MASK;

	str = g_string_new(NULL);
	while (**format != '\0') {
		if ((flags & EXPAND_FLAG_ROOT) == 0 && **format == '}') {
			/* ignore } if we're expanding original string */
			(*format)++;
			break;
		}

		if (**format != '{') {
			if ((flags & EXPAND_FLAG_LASTCOLOR_ARG) &&
			    **format == '$' && (*format)[1] == '0') {
				/* save the color before $0 ..
				   this is for the %n replacing */
				if (save_last_fg != NULL) {
					*save_last_fg = last_fg;
					save_last_fg = NULL;
				}
				if (save_last_bg != NULL) {
					*save_last_bg = last_bg;
					save_last_bg = NULL;
				}
			}

			theme_format_append_next(theme, str, format,
						 default_fg, default_bg,
						 &last_fg, &last_bg,
						 recurse_flags);
			continue;
		}

		(*format)++;
		if (**format == '\0' || **format == '}')
			break; /* error */

		/* get a single {...} */
		abstract = theme_format_expand_abstract(theme, format, &last_fg, &last_bg,
		                                        recurse_flags, block_list);
		if (abstract != NULL) {
			g_string_append(str, abstract);
			g_free(abstract);
		}
	}

		/* save the last color */
		if (save_last_fg != NULL)
			*save_last_fg = last_fg;
		if (save_last_bg != NULL)
			*save_last_bg = last_bg;

	ret = str->str;
        g_string_free(str, FALSE);
        return ret;
}