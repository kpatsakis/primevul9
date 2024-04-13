_connect_common (NMVpnServicePlugin   *plugin,
                 gboolean       interactive,
                 NMConnection  *connection,
                 GVariant      *details,
                 GError       **error)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);
	NMSettingVpn *s_vpn;
	NMSettingConnection *s_con;
	char end[] = { 0x04 };
	gs_free char *bus_name = NULL;

	s_con = nm_connection_get_setting_connection (connection);
	g_assert (s_con);

	s_vpn = nm_connection_get_setting_vpn (connection);
	g_assert (s_vpn);

	if (!nm_vpnc_properties_validate (s_vpn, error))
		goto out;

	if (!nm_vpnc_secrets_validate (s_vpn, interactive, error))
		goto out;

	priv->pid_file = g_strdup_printf (NM_VPNC_PID_PATH "/nm-vpnc-%s.pid", nm_connection_get_uuid (connection));

	if (!nm_vpnc_start_vpnc_binary (NM_VPNC_PLUGIN (plugin), interactive, error))
		goto out;

	if (_LOGD_enabled () || getenv ("NM_VPNC_DUMP_CONNECTION")) {
		_LOGD ("connection:");
		nm_connection_dump (connection);
	}

	g_object_get (plugin, NM_VPN_SERVICE_PLUGIN_DBUS_SERVICE_NAME, &bus_name, NULL);
	if (!nm_vpnc_config_write (priv->infd, bus_name, s_con, s_vpn, error))
		goto out;

	if (interactive) {
		if (write (priv->infd, &end, sizeof (end)) < 0)
			_LOGW ("Unexpected error in write(): %d", errno);
	} else {
		close (priv->infd);
		priv->infd = -1;
	}

	return TRUE;

out:
	vpnc_cleanup (NM_VPNC_PLUGIN (plugin), TRUE);
	return FALSE;
}