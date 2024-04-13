real_need_secrets (NMVpnServicePlugin *plugin,
                   NMConnection *connection,
                   const char **out_setting_name,
                   GError **error)
{
	NMSettingVpn *s_vpn;
	NMSettingSecretFlags pw_flags;
	const char *pw = NULL;

	g_return_val_if_fail (NM_IS_VPN_SERVICE_PLUGIN (plugin), FALSE);
	g_return_val_if_fail (NM_IS_CONNECTION (connection), FALSE);

	s_vpn = nm_connection_get_setting_vpn (connection);
	if (!s_vpn) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_INVALID_CONNECTION,
		             "%s",
		             _("Could not process the request because the VPN connection settings were invalid."));
		return FALSE;
	}

	/* User password */
	pw = nm_setting_vpn_get_secret (s_vpn, NM_VPNC_KEY_SECRET);
	pw_flags = get_pw_flags (s_vpn, NM_VPNC_KEY_SECRET, NM_VPNC_KEY_SECRET_TYPE);
	if (!pw && !(pw_flags & NM_SETTING_SECRET_FLAG_NOT_REQUIRED)) {
		*out_setting_name = NM_SETTING_VPN_SETTING_NAME;
		return TRUE;
	}

	/* Group password */
	pw = nm_setting_vpn_get_secret (s_vpn, NM_VPNC_KEY_XAUTH_PASSWORD);
	pw_flags = get_pw_flags (s_vpn, NM_VPNC_KEY_XAUTH_PASSWORD, NM_VPNC_KEY_XAUTH_PASSWORD_TYPE);
	if (!pw && !(pw_flags & NM_SETTING_SECRET_FLAG_NOT_REQUIRED)) {
		*out_setting_name = NM_SETTING_VPN_SETTING_NAME;
		return TRUE;
	}

	return FALSE;
}