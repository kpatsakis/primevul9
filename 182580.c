main (int argc, char *argv[])
{
	NMVPNCPlugin *plugin;
	gboolean persist = FALSE;
	GOptionContext *opt_ctx = NULL;
	gs_free char *bus_name_free = NULL;
	const char *bus_name;
	GError *error = NULL;

	GOptionEntry options[] = {
		{ "persist", 0, 0, G_OPTION_ARG_NONE, &persist, N_("Don’t quit when VPN connection terminates"), NULL },
		{ "debug", 0, 0, G_OPTION_ARG_NONE, &gl.debug, N_("Enable verbose debug logging (may expose passwords)"), NULL },
		{ "bus-name", 0, 0, G_OPTION_ARG_STRING, &bus_name_free, N_("D-Bus name to use for this instance"), NULL },
		{NULL}
	};

#if !GLIB_CHECK_VERSION (2, 35, 0)
	g_type_init ();
#endif

	/* locale will be set according to environment LC_* variables */
	setlocale (LC_ALL, "");

	bindtextdomain (GETTEXT_PACKAGE, NM_VPNC_LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Parse options */
	opt_ctx = g_option_context_new (NULL);
	g_option_context_set_translation_domain (opt_ctx, GETTEXT_PACKAGE);
	g_option_context_set_ignore_unknown_options (opt_ctx, FALSE);
	g_option_context_set_help_enabled (opt_ctx, TRUE);
	g_option_context_add_main_entries (opt_ctx, options, NULL);

	g_option_context_set_summary (opt_ctx,
	                              _("nm-vpnc-service provides integrated "
	                                "Cisco Legacy IPsec VPN capability to NetworkManager."));

	if (!g_option_context_parse (opt_ctx, &argc, &argv, &error)) {
		g_printerr ("Error parsing the command line options: %s\n", error->message);
		g_option_context_free (opt_ctx);
		g_clear_error (&error);
		exit (EXIT_FAILURE);
	}

	g_option_context_free (opt_ctx);

	bus_name = bus_name_free ?: NM_DBUS_SERVICE_VPNC;
	if (!g_dbus_is_name (bus_name)) {
		g_printerr ("invalid --bus-name\n");
		exit (EXIT_FAILURE);
	}

	interactive_available = vpnc_check_interactive ();

	if (getenv ("VPNC_DEBUG"))
		gl.debug = TRUE;

	gl.log_level = _nm_utils_ascii_str_to_int64 (getenv ("NM_VPN_LOG_LEVEL"),
	                                             10, 0, LOG_DEBUG, -1);

	if (gl.log_level < 0)
		gl.log_level_native = gl.debug ? 3 : 0;
	else if (gl.log_level <= 0)
		gl.log_level_native = 0;
	else if (gl.log_level <= LOG_WARNING)
		gl.log_level_native = 1;
	else if (gl.log_level <= LOG_NOTICE)
		gl.log_level_native = 2;
	else if (gl.log_level <= LOG_INFO)
		gl.log_level_native = 3;
	else {
		/* level 99 prints passwords. We don't want that even for the highest
		 * level. So, choose one below. */
		gl.log_level_native = 98;
	}

	if (gl.log_level < 0)
		gl.log_level = gl.debug ? LOG_DEBUG : LOG_NOTICE;

	_LOGD ("nm-vpnc-service (version " DIST_VERSION ") starting...");
	_LOGD ("   vpnc interactive mode is %s", interactive_available ? "enabled" : "disabled");
	_LOGD ("   uses%s --bus-name \"%s\"", bus_name_free ? "" : " default", bus_name);

	if (system ("/sbin/modprobe tun") == -1)
		exit (EXIT_FAILURE);

	plugin = nm_vpnc_plugin_new (bus_name);
	if (!plugin)
		exit (EXIT_FAILURE);

	_LOGD ("nm-vpnc-service (version " DIST_VERSION ") started.");

	gl.loop = g_main_loop_new (NULL, FALSE);

	if (!persist)
		g_signal_connect (plugin, "quit", G_CALLBACK (quit_mainloop), gl.loop);

	setup_signals ();
	g_main_loop_run (gl.loop);

	remove_pidfile (plugin);

	g_main_loop_unref (gl.loop);
	gl.loop = NULL;
	g_object_unref (plugin);

	exit (EXIT_SUCCESS);
}