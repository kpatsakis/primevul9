real_connect_interactive (NMVpnServicePlugin   *plugin,
                          NMConnection  *connection,
                          GVariant      *details,
                          GError       **error)
{
	if (!interactive_available) {
		g_set_error_literal (error,
		                     NM_VPN_PLUGIN_ERROR,
		                     NM_VPN_PLUGIN_ERROR_INTERACTIVE_NOT_SUPPORTED,
		                     _("vpnc does not support interactive requests"));
		return FALSE;
	}

	if (!_connect_common (plugin, TRUE, connection, details, error))
		return FALSE;

	NM_VPNC_PLUGIN_GET_PRIVATE (plugin)->interactive = TRUE;
	return TRUE;
}