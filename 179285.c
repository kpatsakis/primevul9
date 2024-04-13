imapx_server_stash_command_arguments (CamelIMAPXServer *is)
{
	GString *buffer;

	/* Stash some reusable capability-based command arguments. */

	buffer = g_string_new ("MESSAGES UNSEEN UIDVALIDITY UIDNEXT");
	if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, CONDSTORE))
		g_string_append (buffer, " HIGHESTMODSEQ");
	g_free (is->priv->status_data_items);
	is->priv->status_data_items = g_string_free (buffer, FALSE);

	g_free (is->priv->list_return_opts);
	if (!is->priv->is_cyrus && CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, LIST_EXTENDED)) {
		buffer = g_string_new ("CHILDREN SUBSCRIBED");
		if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, LIST_STATUS))
			g_string_append_printf (
				buffer, " STATUS (%s)",
				is->priv->status_data_items);
		if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, SPECIAL_USE))
			g_string_append_printf (buffer, " SPECIAL-USE");
		is->priv->list_return_opts = g_string_free (buffer, FALSE);
	} else {
		is->priv->list_return_opts = NULL;
	}
}