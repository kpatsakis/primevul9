vpnc_prompt (const char *data, gsize dlen, gpointer user_data)
{
	NMVPNCPlugin *plugin = NM_VPNC_PLUGIN (user_data);
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);
	const char *hints[2] = { NULL, NULL };
	char *prompt;
	guint i;

	g_warn_if_fail (priv->pending_auth == NULL);
	priv->pending_auth = NULL;

	prompt = g_strndup (data, dlen);
	_LOGD ("vpnc requested input: '%s'", prompt);
	for (i = 0; i < G_N_ELEMENTS (phmap); i++) {
		if (g_str_has_prefix (prompt, phmap[i].prompt)) {
			hints[0] = phmap[i].hint;
			break;
		}
	}

	if (!hints[0]) {
		_LOGD ("Unhandled vpnc message '%s'", prompt);
		g_free (prompt);
		return;
	}

	_LOGD ("Requesting new secrets: '%s' (%s)", prompt, hints[0]);

	nm_vpn_service_plugin_secrets_required (NM_VPN_SERVICE_PLUGIN (plugin),
	                                priv->server_message->len ? priv->server_message->str : prompt,
	                                (const char **) hints);
	g_string_truncate (priv->server_message, 0);
	g_free (prompt);

	priv->pending_auth = hints[0];
}