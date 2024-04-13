vpnc_check_interactive (void)
{
	const char *vpnc_path;
	const char *argv[3];
	GError *error = NULL;
	char *output = NULL;
	gboolean has_interactive = FALSE;

	vpnc_path = find_vpnc ();
	if (!vpnc_path) {
		_LOGW ("Failed to find vpnc for version check");
		return FALSE;
	}

	argv[0] = vpnc_path;
	argv[1] = "--long-help";
	argv[2] = NULL;
	if (g_spawn_sync ("/", (char **) argv, NULL, G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL, &output, NULL, NULL, &error)) {
		if (strstr (output, "--password-helper"))
			has_interactive = TRUE;
		g_free (output);
	} else {
		_LOGW ("Failed to start vpnc for version check: %s", error->message);
		g_error_free (error);
	}

	return has_interactive;
}