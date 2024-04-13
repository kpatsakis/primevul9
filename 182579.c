nm_vpnc_plugin_new (const char *bus_name)
{
	NMVPNCPlugin *plugin;
	GError *error = NULL;

	plugin = (NMVPNCPlugin *) g_initable_new (NM_TYPE_VPNC_PLUGIN, NULL, &error,
	                                          NM_VPN_SERVICE_PLUGIN_DBUS_SERVICE_NAME, bus_name,
	                                          NM_VPN_SERVICE_PLUGIN_DBUS_WATCH_PEER, !gl.debug,
	                                          NULL);
	if (!plugin) {
		_LOGW ("Failed to initialize a plugin instance: %s", error->message);
		g_error_free (error);
	}

	return plugin;
}