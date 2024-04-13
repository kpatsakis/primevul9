nm_vpnc_start_vpnc_binary (NMVPNCPlugin *plugin, gboolean interactive, GError **error)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);
	const char *vpnc_path;
	const char *args[10];
	guint i = 0;

	g_return_val_if_fail (priv->pid == 0, FALSE);
	g_return_val_if_fail (priv->infd == -1, FALSE);
	g_return_val_if_fail (priv->out.fd == -1, FALSE);
	g_return_val_if_fail (priv->err.fd == -1, FALSE);

	vpnc_path = find_vpnc ();
	if (!vpnc_path) {
		g_set_error_literal (error,
		                     NM_VPN_PLUGIN_ERROR,
		                     NM_VPN_PLUGIN_ERROR_LAUNCH_FAILED,
		                     _("Could not find vpnc binary."));
		return FALSE;
	}

	args[i++] = vpnc_path;
	args[i++] = "--no-detach";
	args[i++] = "--pid-file";
	args[i++] = priv->pid_file;
	if (!interactive)
		args[i++] = "--non-inter";
	args[i++] = "-";
	args[i++] = NULL;
	if (!g_spawn_async_with_pipes (NULL,
	                               (char **) args,
	                               NULL,
	                               G_SPAWN_DO_NOT_REAP_CHILD,
	                               NULL,
	                               NULL,
	                               &priv->pid,
	                               &priv->infd,
	                               interactive ? &priv->out.fd : NULL,
	                               interactive ? &priv->err.fd : NULL,
	                               error)) {
		_LOGW ("vpnc failed to start.  error: '%s'", (*error)->message);
		return FALSE;
	}
	_LOGI ("vpnc started with pid %d", priv->pid);

	priv->watch_id = g_child_watch_add (priv->pid, vpnc_watch_cb, plugin);

	if (interactive) {
		/* Watch stdout and stderr */
		pipe_setup (&priv->out, stdout, plugin);
		pipe_setup (&priv->err, stderr, plugin);
	}
	return TRUE;
}