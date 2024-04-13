e_util_get_source_full_name (ESourceRegistry *registry,
			     ESource *source)
{
	GString *fullname;
	GSList *parts, *link;

	g_return_val_if_fail (E_IS_SOURCE (source), NULL);

	if (!registry)
		return g_strdup (e_source_get_display_name (source));

	parts = NULL;

	parts = g_slist_prepend (parts, g_strdup (e_source_get_display_name (source)));

	g_object_ref (source);
	while (source) {
		const gchar *parent_id;
		ESource *parent;

		parent_id = e_source_get_parent (source);
		if (!parent_id || !*parent_id)
			break;

		parent = e_source_registry_ref_source (registry, parent_id);
		g_object_unref (source);
		source = parent;

		if (source) {
			const gchar *display_name = e_source_get_display_name (source);

			if (!display_name || !*display_name)
				break;

			parts = g_slist_prepend (parts, g_strdup (display_name));
		}
	}

	g_object_unref (source);

	fullname = g_string_new ("");

	for (link = parts; link; link = link->next) {
		const gchar *part = link->data;

		if (fullname->len) {
			if (link == parts->next)
				g_string_append (fullname, " : ");
			else
				g_string_append_c (fullname, '/');
		}

		g_string_append (fullname, part);
	}

	g_slist_free_full (parts, g_free);

	return g_string_free (fullname, FALSE);
}