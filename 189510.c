e_util_replace_prefix (const gchar *configure_time_prefix,
                       const gchar *runtime_prefix,
                       const gchar *configure_time_path)
{
	gchar *c_t_prefix_slash;
	gchar *retval;

	c_t_prefix_slash = g_strconcat (configure_time_prefix, "/", NULL);

	if (runtime_prefix &&
	    !g_str_has_prefix (configure_time_path, c_t_prefix_slash)) {
		gint ii;
		gchar *path;

		path = g_strdup (configure_time_path);

		for (ii = 0; ii < 3; ii++) {
			const gchar *pos;
			gchar *last_slash;

			last_slash = strrchr (path, '/');
			if (!last_slash)
				break;

			*last_slash = '\0';

			pos = strstr (configure_time_prefix, path);
			if (pos && pos[strlen(path)] == '/') {
				g_free (c_t_prefix_slash);
				c_t_prefix_slash = g_strconcat (configure_time_prefix + (pos - configure_time_prefix), "/", NULL);
				break;
			}
		}

		g_free (path);
	}

	if (runtime_prefix &&
	    g_str_has_prefix (configure_time_path, c_t_prefix_slash)) {
		retval = g_strconcat (
			runtime_prefix,
			configure_time_path + strlen (c_t_prefix_slash) - 1,
			NULL);
	} else
		retval = g_strdup (configure_time_path);

	g_free (c_t_prefix_slash);

	return retval;
}