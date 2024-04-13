parse_color (const gchar *ptr,
	     gdouble     *color,
	     gint         n_color)
{
	gchar *p = (gchar *)ptr;
	gint   i;

	for (i = 0; i < n_color; i++) {
		while (isspace (*p)) p++;
		color[i] = g_ascii_strtod (p, NULL);
		while (!isspace (*p) && *p != '\0') p++;
		if (*p == '\0')
			break;
	}
}