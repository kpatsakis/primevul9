camel_imapx_server_list_sync (CamelIMAPXServer *is,
			      const gchar *in_pattern,
			      CamelStoreGetFolderInfoFlags flags,
			      GCancellable *cancellable,
			      GError **error)
{
	CamelIMAPXCommand *ic;
	gchar *utf7_pattern = NULL;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (in_pattern != NULL, FALSE);

	g_warn_if_fail (is->priv->list_responses_hash == NULL);
	g_warn_if_fail (is->priv->list_responses == NULL);
	g_warn_if_fail (is->priv->lsub_responses == NULL);

	if (!camel_imapx_server_get_utf8_accept (is))
		utf7_pattern = camel_utf8_utf7 (in_pattern);

	if (is->priv->list_return_opts != NULL) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LIST, "LIST \"\" %s RETURN (%t)",
			utf7_pattern ? utf7_pattern : in_pattern, is->priv->list_return_opts);
	} else {
		is->priv->list_responses_hash = g_hash_table_new (camel_strcase_hash, camel_strcase_equal);

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LIST, "LIST \"\" %s",
			utf7_pattern ? utf7_pattern : in_pattern);
	}

	success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching folders"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (success && !is->priv->list_return_opts) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LSUB, "LSUB \"\" %s",
			utf7_pattern ? utf7_pattern : in_pattern);

		success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching subscribed folders"), cancellable, error);

		camel_imapx_command_unref (ic);
	}

	g_free (utf7_pattern);

	if (is->priv->list_responses_hash) {
		CamelIMAPXStore *imapx_store;
		GSList *link;

		imapx_store = camel_imapx_server_ref_store (is);
		if (imapx_store) {
			/* Preserve order in which these had been received from the server */
			is->priv->list_responses = g_slist_reverse (is->priv->list_responses);
			is->priv->lsub_responses = g_slist_reverse (is->priv->lsub_responses);

			for (link = is->priv->list_responses; link; link = g_slist_next (link)) {
				CamelIMAPXListResponse *response = link->data;

				camel_imapx_store_handle_list_response (imapx_store, is, response);
			}

			for (link = is->priv->lsub_responses; link; link = g_slist_next (link)) {
				CamelIMAPXListResponse *response = link->data;

				camel_imapx_store_handle_lsub_response (imapx_store, is, response);
			}

			g_clear_object (&imapx_store);
		}

		g_hash_table_destroy (is->priv->list_responses_hash);
		is->priv->list_responses_hash = NULL;
		g_slist_free_full (is->priv->list_responses, g_object_unref);
		is->priv->list_responses = NULL;
		g_slist_free_full (is->priv->lsub_responses, g_object_unref);
		is->priv->lsub_responses = NULL;
	}

	return success;
}