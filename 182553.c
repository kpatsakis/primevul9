real_new_secrets (NMVpnServicePlugin *plugin,
                  NMConnection *connection,
                  GError **error)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);
	NMSettingVpn *s_vpn;
	const char *secret;

	if (!interactive_available || !priv->interactive) {
		g_set_error_literal (error,
		                     NM_VPN_PLUGIN_ERROR,
		                     NM_VPN_PLUGIN_ERROR_FAILED,
		                     _("Could not use new secrets as interactive mode is disabled."));
		return FALSE;
	}

	s_vpn = nm_connection_get_setting_vpn (connection);
	if (!s_vpn) {
		g_set_error_literal (error,
		                     NM_VPN_PLUGIN_ERROR,
		                     NM_VPN_PLUGIN_ERROR_INVALID_CONNECTION,
		                     _("Could not process the request because the VPN connection settings were invalid."));
		return FALSE;
	}

	if (!priv->pending_auth) {
		g_set_error_literal (error,
		                     NM_VPN_PLUGIN_ERROR,
		                     NM_VPN_PLUGIN_ERROR_INVALID_CONNECTION,
		                     _("Could not process the request because no pending authentication is required."));
		return FALSE;
	}

	_LOGD ("VPN received new secrets; sending to '%s' vpnc stdin", priv->pending_auth);

	secret = nm_setting_vpn_get_secret (s_vpn, priv->pending_auth);
	if (!secret) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_INVALID_CONNECTION,
		             _("Could not process the request because the requested info “%s” was not provided."),
		             priv->pending_auth);
		return FALSE;
	}

	/* Ignoring secret flags here; if vpnc requested the item, we must provide it */
	write_config_option (priv->infd, "%s", secret);

	priv->pending_auth = NULL;
	return TRUE;
}