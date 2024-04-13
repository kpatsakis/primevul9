replace_untagged_descriptor (GHashTable *untagged_handlers,
                             const gchar *key,
                             const CamelIMAPXUntaggedRespHandlerDesc *descr)
{
	const CamelIMAPXUntaggedRespHandlerDesc *prev = NULL;

	g_return_val_if_fail (untagged_handlers != NULL, NULL);
	g_return_val_if_fail (key != NULL, NULL);
	/* descr may be NULL (to delete a handler) */

	prev = g_hash_table_lookup (untagged_handlers, key);
	g_hash_table_replace (
		untagged_handlers,
		g_strdup (key),
		(gpointer) descr);
	return prev;
}