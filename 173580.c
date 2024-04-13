static SC_HTMLTag *sc_html_get_tag(const gchar *str)
{
	SC_HTMLTag *tag;
	gchar *tmp;
	guchar *tmpp;

	cm_return_val_if_fail(str != NULL, NULL);

	if (*str == '\0' || *str == '!') return NULL;

	Xstrdup_a(tmp, str, return NULL);

	tag = g_new0(SC_HTMLTag, 1);

	for (tmpp = tmp; *tmpp != '\0' && !g_ascii_isspace(*tmpp); tmpp++)
		;

	if (*tmpp == '\0') {
		tag->name = g_utf8_strdown(tmp, -1);
		return tag;
	} else {
		*tmpp++ = '\0';
		tag->name = g_utf8_strdown(tmp, -1);
	}

	while (*tmpp != '\0') {
		SC_HTMLAttr *attr;
		gchar *attr_name;
		gchar *attr_value;
		gchar *p;
		gchar quote;

		while (g_ascii_isspace(*tmpp)) tmpp++;
		attr_name = tmpp;

		while (*tmpp != '\0' && !g_ascii_isspace(*tmpp) &&
		       *tmpp != '=')
			tmpp++;
		if (*tmpp != '\0' && *tmpp != '=') {
			*tmpp++ = '\0';
			while (g_ascii_isspace(*tmpp)) tmpp++;
		}

		if (*tmpp == '=') {
			*tmpp++ = '\0';
			while (g_ascii_isspace(*tmpp)) tmpp++;

			if (*tmpp == '"' || *tmpp == '\'') {
				/* name="value" */
				quote = *tmpp;
				tmpp++;
				attr_value = tmpp;
				if ((p = strchr(attr_value, quote)) == NULL) {
					g_warning("sc_html_get_tag(): syntax error in tag: '%s'\n", str);
					return tag;
				}
				tmpp = p;
				*tmpp++ = '\0';
				while (g_ascii_isspace(*tmpp)) tmpp++;
			} else {
				/* name=value */
				attr_value = tmpp;
				while (*tmpp != '\0' && !g_ascii_isspace(*tmpp)) tmpp++;
				if (*tmpp != '\0')
					*tmpp++ = '\0';
			}
		} else
			attr_value = "";

		g_strchomp(attr_name);
		attr = g_new(SC_HTMLAttr, 1);
		attr->name = g_utf8_strdown(attr_name, -1);
		attr->value = g_strdup(attr_value);
		tag->attr = g_list_append(tag->attr, attr);
	}

	return tag;
}