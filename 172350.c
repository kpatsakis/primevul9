connect_to_server_process (CamelIMAPXServer *is,
                           const gchar *cmd,
                           GError **error)
{
	GSubprocessLauncher *launcher;
	GSubprocess *subprocess = NULL;
	CamelNetworkSettings *network_settings;
	CamelProvider *provider;
	CamelSettings *settings;
	CamelIMAPXStore *store;
	CamelURL url;
	gchar **argv = NULL;
	gchar *buf;
	gchar *cmd_copy;
	gchar *full_cmd;
	const gchar *password;
	gchar *host;
	gchar *user;
	guint16 port;

	memset (&url, 0, sizeof (CamelURL));

	launcher = g_subprocess_launcher_new (
		G_SUBPROCESS_FLAGS_STDIN_PIPE |
		G_SUBPROCESS_FLAGS_STDOUT_PIPE |
		G_SUBPROCESS_FLAGS_STDERR_SILENCE);

#ifdef G_OS_UNIX
	g_subprocess_launcher_set_child_setup (
		launcher, imapx_server_child_process_setup,
		NULL, (GDestroyNotify) NULL);
#endif

	store = camel_imapx_server_ref_store (is);

	password = camel_service_get_password (CAMEL_SERVICE (store));
	provider = camel_service_get_provider (CAMEL_SERVICE (store));
	settings = camel_service_ref_settings (CAMEL_SERVICE (store));

	network_settings = CAMEL_NETWORK_SETTINGS (settings);
	host = camel_network_settings_dup_host (network_settings);
	port = camel_network_settings_get_port (network_settings);
	user = camel_network_settings_dup_user (network_settings);

	/* Put full details in the environment, in case the connection
	 * program needs them */
	camel_url_set_protocol (&url, provider->protocol);
	camel_url_set_host (&url, host);
	camel_url_set_port (&url, port);
	camel_url_set_user (&url, user);
	buf = camel_url_to_string (&url, 0);

	g_subprocess_launcher_setenv (launcher, "URL", buf, TRUE);
	g_subprocess_launcher_setenv (launcher, "URLHOST", host, TRUE);

	if (port > 0) {
		gchar *port_string;

		port_string = g_strdup_printf ("%u", port);
		g_subprocess_launcher_setenv (
			launcher, "URLPORT", port_string, TRUE);
		g_free (port_string);
	}

	if (user != NULL) {
		g_subprocess_launcher_setenv (
			launcher, "URLPORT", user, TRUE);
	}

	if (password != NULL) {
		g_subprocess_launcher_setenv (
			launcher, "URLPASSWD", password, TRUE);
	}

	g_free (buf);

	g_object_unref (settings);
	g_object_unref (store);

	/* Now do %h, %u, etc. substitution in cmd */
	buf = cmd_copy = g_strdup (cmd);

	full_cmd = g_strdup ("");

	for (;;) {
		gchar *pc;
		gchar *tmp;
		const gchar *var;
		gint len;

		pc = strchr (buf, '%');
	ignore:
		if (!pc) {
			tmp = g_strdup_printf ("%s%s", full_cmd, buf);
			g_free (full_cmd);
			full_cmd = tmp;
			break;
		}

		len = pc - buf;

		var = NULL;

		switch (pc[1]) {
		case 'h':
			var = host;
			break;
		case 'u':
			var = user;
			break;
		}
		if (!var) {
			/* If there wasn't a valid %-code, with an actual
			 * variable to insert, pretend we didn't see the % */
			pc = strchr (pc + 1, '%');
			goto ignore;
		}
		tmp = g_strdup_printf ("%s%.*s%s", full_cmd, len, buf, var);
		g_free (full_cmd);
		full_cmd = tmp;
		buf = pc + 2;
	}

	g_free (cmd_copy);

	g_free (host);
	g_free (user);

	if (g_shell_parse_argv (full_cmd, NULL, &argv, error)) {
		subprocess = g_subprocess_launcher_spawnv (
			launcher, (const gchar * const *) argv, error);
		g_strfreev (argv);
	}

	g_free (full_cmd);
	g_object_unref (launcher);

	if (subprocess != NULL) {
		GInputStream *input_stream;
		GOutputStream *output_stream;

		g_mutex_lock (&is->priv->stream_lock);
		g_warn_if_fail (is->priv->subprocess == NULL);
		is->priv->subprocess = g_object_ref (subprocess);
		g_mutex_unlock (&is->priv->stream_lock);

		input_stream = g_subprocess_get_stdout_pipe (subprocess);
		output_stream = g_subprocess_get_stdin_pipe (subprocess);

		imapx_server_set_streams (is, input_stream, output_stream);

		g_object_unref (subprocess);
	}

	return TRUE;
}