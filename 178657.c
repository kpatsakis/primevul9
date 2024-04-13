char *theme_format_expand_get(THEME_REC *theme, const char **format)
{
	GString *str;
	char *ret;
	theme_rm_col dummy, reset;
	int braces = 1; /* we start with one brace opened */
	dummy.m[0] = '\0';
	strcpy(reset.m, "n");

	str = g_string_new(NULL);
	while (**format != '\0' && braces != 0) {
		if (**format == '{')
			braces++;
		else if (**format == '}')
			braces--;
		else if ((braces > 1) && (**format == ' ')) {
			g_string_append(str, "\\x20");
			(*format)++;
			continue;
		} else {
			theme_format_append_next(theme, str, format,
						 reset, reset,
						 &dummy, &dummy,
						 EXPAND_FLAG_IGNORE_REPLACES);
			continue;
		}

		if (braces == 0) {
			(*format)++;
			break;
		}

		g_string_append_c(str, **format);
		(*format)++;
	}

	ret = str->str;
        g_string_free(str, FALSE);
        return ret;
}