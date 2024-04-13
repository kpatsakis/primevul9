create_initial_untagged_handler_table (void)
{
	GHashTable *uh = g_hash_table_new_full (
		camel_strcase_hash,
		camel_strcase_equal,
		g_free,
		NULL);
	guint32 ii = 0;

	/* CamelIMAPXServer predefined handlers*/
	for (ii = 0; ii < IMAPX_UNTAGGED_LAST_ID; ii++)
		add_initial_untagged_descriptor (uh, ii);

	g_return_val_if_fail (g_hash_table_size (uh) == IMAPX_UNTAGGED_LAST_ID, NULL);

	return uh;
}