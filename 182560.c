find_vpnc (void)
{
	static const char *vpnc_paths[] = {
		"/usr/sbin/vpnc",
		"/sbin/vpnc",
		"/usr/local/sbin/vpnc",
		NULL
	};
	guint i;

	/* Find vpnc */
	for (i = 0; vpnc_paths[i]; i++) {
		if (g_file_test (vpnc_paths[i], G_FILE_TEST_EXISTS))
			return vpnc_paths[i];
	}
	return NULL;
}