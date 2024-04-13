camel_imapx_server_authenticate_sync (CamelIMAPXServer *is,
				      const gchar *mechanism,
				      GCancellable *cancellable,
				      GError **error)
{
	CamelNetworkSettings *network_settings;
	CamelIMAPXStore *store;
	CamelService *service;
	CamelSettings *settings;
	CamelAuthenticationResult result;
	CamelIMAPXCommand *ic;
	CamelSasl *sasl = NULL;
	gchar *host;
	gchar *user;

	g_return_val_if_fail (
		CAMEL_IS_IMAPX_SERVER (is),
		CAMEL_AUTHENTICATION_ERROR);

	store = camel_imapx_server_ref_store (is);

	service = CAMEL_SERVICE (store);
	settings = camel_service_ref_settings (service);

	network_settings = CAMEL_NETWORK_SETTINGS (settings);
	host = camel_network_settings_dup_host (network_settings);
	user = camel_network_settings_dup_user (network_settings);

	g_object_unref (settings);

	if (mechanism != NULL) {
		g_mutex_lock (&is->priv->stream_lock);

		if (is->priv->cinfo && !g_hash_table_lookup (is->priv->cinfo->auth_types, mechanism) && (
		    !g_str_equal (mechanism, "Google") || !g_hash_table_lookup (is->priv->cinfo->auth_types, "XOAUTH2"))) {
			g_mutex_unlock (&is->priv->stream_lock);
			g_set_error (
				error, CAMEL_SERVICE_ERROR,
				CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
				_("IMAP server %s does not support %s "
				"authentication"), host, mechanism);
			result = CAMEL_AUTHENTICATION_ERROR;
			goto exit;
		} else {
			g_mutex_unlock (&is->priv->stream_lock);
		}

		sasl = camel_sasl_new ("imap", mechanism, service);
		if (sasl == NULL) {
			g_set_error (
				error, CAMEL_SERVICE_ERROR,
				CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
				_("No support for %s authentication"),
				mechanism);
			result = CAMEL_AUTHENTICATION_ERROR;
			goto exit;
		}
	}

	if (sasl != NULL) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_AUTHENTICATE, "AUTHENTICATE %A", sasl);
	} else {
		const gchar *password;

		password = camel_service_get_password (service);

		if (user == NULL) {
			g_set_error_literal (
				error, CAMEL_SERVICE_ERROR,
				CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
				_("Cannot authenticate without a username"));
			result = CAMEL_AUTHENTICATION_ERROR;
			goto exit;
		}

		if (password == NULL) {
			g_set_error_literal (
				error, CAMEL_SERVICE_ERROR,
				CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
				_("Authentication password not available"));
			result = CAMEL_AUTHENTICATION_ERROR;
			goto exit;
		}

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LOGIN, "LOGIN %s %s", user, password);
	}

	if (!camel_imapx_server_process_command_sync (is, ic, _("Failed to authenticate"), cancellable, error) && (
	    !ic->status || ic->status->result != IMAPX_NO))
		result = CAMEL_AUTHENTICATION_ERROR;
	else if (ic->status->result == IMAPX_OK)
		result = CAMEL_AUTHENTICATION_ACCEPTED;
	else if (ic->status->result == IMAPX_NO) {
		g_clear_error (error);

		if (camel_imapx_store_is_connecting_concurrent_connection (store)) {
			/* At least one connection succeeded, probably max connection limit
			   set on the server had been reached, thus use special error code
			   for it, to instruct the connection manager to decrease the limit
			   and use already created connection. */
			g_set_error_literal (
				error, CAMEL_IMAPX_SERVER_ERROR,
				CAMEL_IMAPX_SERVER_ERROR_CONCURRENT_CONNECT_FAILED,
				ic->status->text ? ic->status->text : _("Unknown error"));
			result = CAMEL_AUTHENTICATION_ERROR;
		} else if (sasl) {
			CamelSaslClass *sasl_class;

			sasl_class = CAMEL_SASL_GET_CLASS (sasl);
			if (sasl_class && sasl_class->auth_type && !sasl_class->auth_type->need_password) {
				g_set_error_literal (
					error, CAMEL_SERVICE_ERROR,
					CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
					ic->status->text ? ic->status->text : _("Unknown error"));
				result = CAMEL_AUTHENTICATION_ERROR;
			} else {
				result = CAMEL_AUTHENTICATION_REJECTED;
			}
		} else {
			result = CAMEL_AUTHENTICATION_REJECTED;
		}
	} else {
		g_set_error_literal (
			error, CAMEL_SERVICE_ERROR,
			CAMEL_SERVICE_ERROR_CANT_AUTHENTICATE,
			ic->status->text ? ic->status->text : _("Unknown error"));
		result = CAMEL_AUTHENTICATION_ERROR;
	}

	/* Forget old capabilities after login. */
	if (result == CAMEL_AUTHENTICATION_ACCEPTED) {
		g_mutex_lock (&is->priv->stream_lock);

		if (is->priv->cinfo) {
			imapx_free_capability (is->priv->cinfo);
			is->priv->cinfo = NULL;
		}

		if (ic->status->condition == IMAPX_CAPABILITY) {
			is->priv->cinfo = ic->status->u.cinfo;
			ic->status->u.cinfo = NULL;
			c (is->priv->tagprefix, "got capability flags %08x\n", is->priv->cinfo ? is->priv->cinfo->capa : 0xFFFFFFFF);
			imapx_server_stash_command_arguments (is);
		}

		g_mutex_unlock (&is->priv->stream_lock);
	}

	camel_imapx_command_unref (ic);

	if (sasl != NULL)
		g_object_unref (sasl);

exit:
	g_free (host);
	g_free (user);

	g_object_unref (store);

	return result;
}