write_one_property (const char *key, const char *value, gpointer user_data)
{
	WriteConfigInfo *info = (WriteConfigInfo *) user_data;
	guint32 type = ITEM_TYPE_UNKNOWN;
	int i;

	if (info->error)
		return;

	/* Find the value in the table to get its type */
	for (i = 0; valid_properties[i].name; i++) {
		ValidProperty prop = valid_properties[i];

		if (!strcmp (prop.name, (char *) key)) {
			/* Property is ok */
			type = prop.type;
			break;
		}
	}

	/* Try the valid secrets table */
	for (i = 0; type == ITEM_TYPE_UNKNOWN && valid_secrets[i].name; i++) {
		ValidProperty prop = valid_secrets[i];

		if (!strcmp (prop.name, (char *) key)) {
			/* Property is ok */
			type = prop.type;
			break;
		}
	}

	if (type == ITEM_TYPE_UNKNOWN) {
		g_set_error (&info->error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_BAD_ARGUMENTS,
		             _("Config option “%s” invalid or unknown."),
		             (const char *) key);
		return;
	}

	/* Don't write ignored secrets */
	if (!strcmp (key, NM_VPNC_KEY_XAUTH_PASSWORD) && info->upw_ignored)
		return;
	if (!strcmp (key, NM_VPNC_KEY_SECRET) && info->gpw_ignored)
		return;

	if (type == ITEM_TYPE_STRING || type == ITEM_TYPE_PATH)
		write_config_option (info->fd, "%s %s", (char *) key, (char *) value);
	else if (type == ITEM_TYPE_SECRET)
		write_config_option_secret (info->fd, key, value);
	else if (type == ITEM_TYPE_BOOLEAN) {
		if (!strcmp (value, "yes"))
			write_config_option (info->fd, "%s", (char *) key);
	} else if (type == ITEM_TYPE_INT) {
		long int tmp_int;
		char *tmp_str;

		/* Convert -> int and back to string for security's sake since
		 * strtol() ignores leading and trailing characters.
		 */
		errno = 0;
		tmp_int = strtol (value, NULL, 10);
		if (errno == 0) {
			tmp_str = g_strdup_printf ("%ld", tmp_int);
			write_config_option (info->fd, "%s %s", (char *) key, tmp_str);
			g_free (tmp_str);
		} else {
			g_set_error (&info->error,
			             NM_VPN_PLUGIN_ERROR,
			             NM_VPN_PLUGIN_ERROR_BAD_ARGUMENTS,
			             _("Config option “%s” not an integer."),
			             (const char *) key);
		}
	} else if (type == ITEM_TYPE_IGNORED) {
		/* ignored */
	} else {
		/* Just ignore unknown properties */
		_LOGW ("Don't know how to write property '%s' with type %d", key, type);
	}
}