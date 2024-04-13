e_timeout_add_seconds_with_name (gint priority,
                                 guint interval,
                                 const gchar *name,
                                 GSourceFunc function,
                                 gpointer data,
                                 GDestroyNotify notify)
{
	guint tag;

	g_return_val_if_fail (function != NULL, 0);

	tag = g_timeout_add_seconds_full (
		priority, interval, function, data, notify);
	g_source_set_name_by_id (tag, name);

	return tag;
}