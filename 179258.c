imapx_reconnect (CamelIMAPXServer *is,
                 GCancellable *cancellable,
                 GError **error)
{
	CamelIMAPXCommand *ic;
	CamelService *service;
	CamelSession *session;
	CamelIMAPXStore *store;
	CamelSettings *settings;
	gchar *mechanism;
	gboolean use_qresync;
	gboolean use_idle;
	gboolean success = FALSE;

	store = camel_imapx_server_ref_store (is);

	service = CAMEL_SERVICE (store);
	session = camel_service_ref_session (service);
	if (!session) {
		g_set_error_literal (
			error, CAMEL_SERVICE_ERROR,
			CAMEL_SERVICE_ERROR_UNAVAILABLE,
			_("You must be working online to complete this operation"));
		g_object_unref (store);
		return FALSE;
	}

	settings = camel_service_ref_settings (service);

	mechanism = camel_network_settings_dup_auth_mechanism (
		CAMEL_NETWORK_SETTINGS (settings));

	use_qresync = camel_imapx_settings_get_use_qresync (CAMEL_IMAPX_SETTINGS (settings));
	use_idle = camel_imapx_settings_get_use_idle (CAMEL_IMAPX_SETTINGS (settings));

	g_object_unref (settings);

	if (!imapx_connect_to_server (is, cancellable, error))
		goto exception;

	if (is->priv->state == IMAPX_AUTHENTICATED)
		goto preauthed;

	if (!camel_session_authenticate_sync (
		session, service, mechanism, cancellable, error))
		goto exception;

	/* After login we re-capa unless the server already told us. */
	g_mutex_lock (&is->priv->stream_lock);
	if (is->priv->cinfo == NULL) {
		GError *local_error = NULL;

		g_mutex_unlock (&is->priv->stream_lock);

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_CAPABILITY, "CAPABILITY");
		camel_imapx_server_process_command_sync (is, ic, _("Failed to get capabilities"), cancellable, &local_error);
		camel_imapx_command_unref (ic);

		if (local_error != NULL) {
			g_propagate_error (error, local_error);
			goto exception;
		}
	} else {
		g_mutex_unlock (&is->priv->stream_lock);
	}

	is->priv->state = IMAPX_AUTHENTICATED;

preauthed:
	/* Fetch namespaces (if supported). */
	g_mutex_lock (&is->priv->stream_lock);
	if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, NAMESPACE)) {
		GError *local_error = NULL;

		g_mutex_unlock (&is->priv->stream_lock);

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_NAMESPACE, "NAMESPACE");
		camel_imapx_server_process_command_sync (is, ic, _("Failed to issue NAMESPACE"), cancellable, &local_error);
		camel_imapx_command_unref (ic);

		if (local_error != NULL) {
			g_propagate_error (error, local_error);
			goto exception;
		}

		g_mutex_lock (&is->priv->stream_lock);
	}

	/* Enable quick mailbox resynchronization (if supported). */
	if (use_qresync && CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, QRESYNC)) {
		GError *local_error = NULL;

		g_mutex_unlock (&is->priv->stream_lock);

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_ENABLE, "ENABLE CONDSTORE QRESYNC");
		camel_imapx_server_process_command_sync (is, ic, _("Failed to enable QResync"), cancellable, &local_error);
		camel_imapx_command_unref (ic);

		if (local_error != NULL) {
			g_propagate_error (error, local_error);
			goto exception;
		}

		g_mutex_lock (&is->priv->stream_lock);

		is->priv->use_qresync = TRUE;
	} else {
		is->priv->use_qresync = FALSE;
	}

	/* Set NOTIFY options after enabling QRESYNC (if supported). */
	if (use_idle && CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, NOTIFY)) {
		GError *local_error = NULL;

		g_mutex_unlock (&is->priv->stream_lock);

		/* XXX The list of FETCH attributes is negotiable. */
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_NOTIFY, "NOTIFY SET "
			"(selected "
			"(MessageNew (UID RFC822.SIZE RFC822.HEADER FLAGS)"
			" MessageExpunge"
			" FlagChange)) "
			"(personal "
			"(MessageNew"
			" MessageExpunge"
			" MailboxName"
			" SubscriptionChange))");
		camel_imapx_server_process_command_sync (is, ic, _("Failed to issue NOTIFY"), cancellable, &local_error);
		camel_imapx_command_unref (ic);

		if (local_error != NULL) {
			g_propagate_error (error, local_error);
			goto exception;
		}

		g_mutex_lock (&is->priv->stream_lock);
	}

	g_mutex_unlock (&is->priv->stream_lock);

	is->priv->state = IMAPX_INITIALISED;

	success = TRUE;

	goto exit;

exception:
	imapx_disconnect (is);

exit:
	g_free (mechanism);

	g_object_unref (session);
	g_object_unref (store);

	return success;
}