imapx_untagged_ok_no_bad (CamelIMAPXServer *is,
                          GInputStream *input_stream,
                          GCancellable *cancellable,
                          GError **error)
{
	CamelIMAPXMailbox *mailbox;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	/* TODO: validate which ones of these can happen as unsolicited responses */
	/* TODO: handle bye/preauth differently */
	camel_imapx_input_stream_ungettoken (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		is->priv->context->tok,
		is->priv->context->token,
		is->priv->context->len);

	/* These untagged responses can belong to ongoing SELECT command, thus
	   to the pending select mailbox, not to the currently selected or closing
	   mailbox, thus prefer the select pending mailbox, from the other two.
	   This makes sure that for example UIDVALIDITY is not incorrectly
	   overwritten with a value from a different mailbox, thus the offline
	   cache will persist, instead of being vanished.
	*/
	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	is->priv->context->sinfo = imapx_parse_status (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		mailbox, cancellable, error);

	g_clear_object (&mailbox);

	if (is->priv->context->sinfo == NULL)
		return FALSE;

	switch (is->priv->context->sinfo->condition) {
	case IMAPX_CLOSED:
		c (
			is->priv->tagprefix,
			"previously selected mailbox is now closed\n");
		{
			CamelIMAPXMailbox *select_mailbox;
			CamelIMAPXMailbox *select_pending;

			g_mutex_lock (&is->priv->select_lock);

			select_mailbox = g_weak_ref_get (&is->priv->select_mailbox);
			select_pending = g_weak_ref_get (&is->priv->select_pending);

			if (select_mailbox == NULL)
				g_weak_ref_set (&is->priv->select_mailbox, select_pending);

			g_mutex_unlock (&is->priv->select_lock);

			g_clear_object (&select_mailbox);
			g_clear_object (&select_pending);
		}
		break;
	case IMAPX_ALERT:
		c (is->priv->tagprefix, "ALERT!: %s\n", is->priv->context->sinfo->text);
		{
			const gchar *alert_message;
			gboolean emit_alert = FALSE;

			g_mutex_lock (&is->priv->known_alerts_lock);

			alert_message = is->priv->context->sinfo->text;

			if (alert_message != NULL) {
				emit_alert = !g_hash_table_contains (
					is->priv->known_alerts,
					alert_message);
			}

			if (emit_alert) {
				CamelIMAPXStore *store;
				CamelService *service;
				CamelSession *session;

				store = camel_imapx_server_ref_store (is);

				g_hash_table_add (
					is->priv->known_alerts,
					g_strdup (alert_message));

				service = CAMEL_SERVICE (store);
				session = camel_service_ref_session (service);

				if (session) {
					camel_session_user_alert (
						session, service,
						CAMEL_SESSION_ALERT_WARNING,
						alert_message);

					g_object_unref (session);
				}

				g_object_unref (store);
			}

			g_mutex_unlock (&is->priv->known_alerts_lock);
		}
		break;
	case IMAPX_PARSE:
		c (is->priv->tagprefix, "PARSE: %s\n", is->priv->context->sinfo->text);
		break;
	case IMAPX_CAPABILITY:
		if (is->priv->context->sinfo->u.cinfo) {
			struct _capability_info *cinfo;

			g_mutex_lock (&is->priv->stream_lock);

			cinfo = is->priv->cinfo;
			is->priv->cinfo = is->priv->context->sinfo->u.cinfo;
			is->priv->context->sinfo->u.cinfo = NULL;
			if (cinfo)
				imapx_free_capability (cinfo);
			c (is->priv->tagprefix, "got capability flags %08x\n", is->priv->cinfo ? is->priv->cinfo->capa : 0xFFFFFFFF);

			if (is->priv->context->sinfo->text) {
				guint32 list_extended = imapx_lookup_capability ("LIST-EXTENDED");

				is->priv->is_cyrus = is->priv->is_cyrus || camel_strstrcase (is->priv->context->sinfo->text, "cyrus");
				if (is->priv->is_cyrus && is->priv->cinfo && (is->priv->cinfo->capa & list_extended) != 0) {
					/* Disable LIST-EXTENDED for cyrus servers */
					c (is->priv->tagprefix, "Disabling LIST-EXTENDED extension for a Cyrus server\n");
					is->priv->cinfo->capa &= ~list_extended;
				}
			}

			imapx_server_stash_command_arguments (is);

			g_mutex_unlock (&is->priv->stream_lock);
		}
		break;
	case IMAPX_COPYUID:
		imapx_free_status (is->priv->copyuid_status);
		is->priv->copyuid_status = is->priv->context->sinfo;
		is->priv->context->sinfo = NULL;
		break;
	default:
		break;
	}

	imapx_free_status (is->priv->context->sinfo);
	is->priv->context->sinfo = NULL;

	return TRUE;
}