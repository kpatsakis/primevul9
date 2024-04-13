imapx_untagged_quota (CamelIMAPXServer *is,
                      GInputStream *input_stream,
                      GCancellable *cancellable,
                      GError **error)
{
	gchar *quota_root_name = NULL;
	CamelFolderQuotaInfo *quota_info = NULL;
	gboolean success;

	success = camel_imapx_parse_quota (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		cancellable, &quota_root_name, &quota_info, error);

	/* Sanity check */
	g_return_val_if_fail (
		(success && (quota_root_name != NULL)) ||
		(!success && (quota_root_name == NULL)), FALSE);

	if (success) {
		CamelIMAPXStore *store;

		store = camel_imapx_server_ref_store (is);
		camel_imapx_store_set_quota_info (
			store, quota_root_name, quota_info);
		g_object_unref (store);

		g_free (quota_root_name);
		camel_folder_quota_info_free (quota_info);
	}

	return success;
}