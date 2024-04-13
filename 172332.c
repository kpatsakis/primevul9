imapx_untagged_recent (CamelIMAPXServer *is,
                       GInputStream *input_stream,
                       GCancellable *cancellable,
                       GError **error)
{
	CamelIMAPXMailbox *mailbox;
	guint32 recent;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	if (mailbox == NULL) {
		g_warning ("%s: No mailbox available", G_STRFUNC);
		return TRUE;
	}

	recent = (guint32) is->priv->context->id;

	c (is->priv->tagprefix, "%s: updating mailbox '%s' recent: %d ~> %d\n", G_STRFUNC,
		camel_imapx_mailbox_get_name (mailbox),
		camel_imapx_mailbox_get_recent (mailbox),
		recent);

	camel_imapx_mailbox_set_recent (mailbox, recent);

	g_object_unref (mailbox);

	return TRUE;
}