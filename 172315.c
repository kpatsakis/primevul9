camel_imapx_server_skip_old_flags_update (CamelStore *store)
{
	CamelSession *session;
	CamelSettings *settings;
	GNetworkMonitor *network_monitor;
	gboolean skip_old_flags_update = FALSE;

	if (!CAMEL_IS_STORE (store))
		return FALSE;

	settings = camel_service_ref_settings (CAMEL_SERVICE (store));
	if (settings) {
		gboolean allow_update;

		allow_update = camel_imapx_settings_get_full_update_on_metered_network (CAMEL_IMAPX_SETTINGS (settings));

		g_object_unref (settings);

		if (allow_update)
			return FALSE;
	}

	session = camel_service_ref_session (CAMEL_SERVICE (store));
	if (!session)
		return skip_old_flags_update;

	network_monitor = camel_session_ref_network_monitor (session);

	skip_old_flags_update = network_monitor && g_network_monitor_get_network_metered (network_monitor);

	g_clear_object (&network_monitor);
	g_clear_object (&session);

	return skip_old_flags_update;
}