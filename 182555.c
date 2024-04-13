nm_vpnc_config_write (gint vpnc_fd,
                      const char *bus_name,
                      NMSettingConnection *s_con,
                      NMSettingVpn *s_vpn,
                      GError **error)
{
	WriteConfigInfo *info;
	const char *props_username;
	const char *props_natt_mode;
	const char *default_username;
	const char *pw_type;
	const char *local_port;
	const char *interface_name;
	NMSettingSecretFlags secret_flags = NM_SETTING_SECRET_FLAG_NONE;

	if (bus_name) {
		g_assert (g_dbus_is_name (bus_name));
		if (nm_streq (bus_name, NM_DBUS_SERVICE_VPNC))
			bus_name = NULL;
	}

	interface_name = nm_setting_connection_get_interface_name(s_con);

	default_username = nm_setting_vpn_get_user_name (s_vpn);

	write_config_option (vpnc_fd, "Debug %d", gl.log_level_native);

	if (interface_name && strlen(interface_name) > 0)
		write_config_option (vpnc_fd, "Interface name %s", interface_name);

	write_config_option (vpnc_fd, "Script %s %d %ld %s%s",
	                     NM_VPNC_HELPER_PATH,
	                     gl.log_level,
	                     (long) getpid(),
	                     bus_name ? " --bus-name " : "", bus_name ?: "");

	write_config_option (vpnc_fd,
	                     NM_VPNC_KEY_CISCO_UDP_ENCAPS_PORT " %d",
	                     NM_VPNC_UDP_ENCAPSULATION_PORT);

	local_port = nm_setting_vpn_get_data_item (s_vpn, NM_VPNC_KEY_LOCAL_PORT);
	if (!local_port) {
		/* Configure 'Local Port' to 0 (random port) if the value is not set in the setting.
		 * Otherwise vpnc would try to use 500 and could clash with other IKE processes.
		 */
		write_config_option (vpnc_fd,
		                     NM_VPNC_KEY_LOCAL_PORT " %d",
		                     NM_VPNC_LOCAL_PORT_ISAKMP);
	}

	/* Fill username if it's not present */
	props_username = nm_setting_vpn_get_data_item (s_vpn, NM_VPNC_KEY_XAUTH_USER);
	if (   default_username
	    && strlen (default_username)
	    && (!props_username || !strlen (props_username))) {
		write_config_option (vpnc_fd,
		                     NM_VPNC_KEY_XAUTH_USER " %s",
		                     default_username);
	}

	/* Use Cisco UDP by default */
	props_natt_mode = nm_setting_vpn_get_data_item (s_vpn, NM_VPNC_KEY_NAT_TRAVERSAL_MODE);
	if (!props_natt_mode || !strlen (props_natt_mode)) {
		write_config_option (vpnc_fd,
		                     NM_VPNC_KEY_NAT_TRAVERSAL_MODE " %s",
		                     NM_VPNC_NATT_MODE_CISCO);
	} else if (props_natt_mode && (!strcmp (props_natt_mode, NM_VPNC_NATT_MODE_NATT_ALWAYS))) {
		write_config_option (vpnc_fd,
		                     NM_VPNC_KEY_NAT_TRAVERSAL_MODE " %s",
		                     NM_VPNC_NATT_MODE_NATT_ALWAYS);
	}

	info = g_malloc0 (sizeof (WriteConfigInfo));
	info->fd = vpnc_fd;

	/* Check for ignored user password */
	if (nm_setting_get_secret_flags (NM_SETTING (s_vpn), NM_VPNC_KEY_XAUTH_PASSWORD, &secret_flags, NULL)) {
		if (secret_flags & NM_SETTING_SECRET_FLAG_NOT_REQUIRED)
			info->upw_ignored = TRUE;
	} else {
		pw_type = nm_setting_vpn_get_data_item (s_vpn, NM_VPNC_KEY_XAUTH_PASSWORD_TYPE);
		if (pw_type && !strcmp (pw_type, NM_VPNC_PW_TYPE_UNUSED))
			info->upw_ignored = TRUE;
	}

	/* Check for ignored group password */
	secret_flags = NM_SETTING_SECRET_FLAG_NONE;
	if (nm_setting_get_secret_flags (NM_SETTING (s_vpn), NM_VPNC_KEY_SECRET, &secret_flags, NULL)) {
		if (secret_flags & NM_SETTING_SECRET_FLAG_NOT_REQUIRED)
			info->gpw_ignored = TRUE;
	} else {
		pw_type = nm_setting_vpn_get_data_item (s_vpn, NM_VPNC_KEY_SECRET_TYPE);
		if (pw_type && !strcmp (pw_type, NM_VPNC_PW_TYPE_UNUSED))
			info->gpw_ignored = TRUE;
	}

	nm_setting_vpn_foreach_data_item (s_vpn, write_one_property, info);
	nm_setting_vpn_foreach_secret (s_vpn, write_one_property, info);
	*error = info->error;
	g_free (info);

	return *error ? FALSE : TRUE;
}