vpnc_cleanup (NMVPNCPlugin *self, gboolean killit)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (self);

	if (priv->infd >= 0) {
		close (priv->infd);
		priv->infd = -1;
	}

	pipe_cleanup (&priv->out);
	pipe_cleanup (&priv->err);
	g_string_truncate (priv->server_message, 0);
	priv->server_message_done = FALSE;

	if (priv->watch_id) {
		g_source_remove (priv->watch_id);
		priv->watch_id = 0;
	}

	if (priv->pid) {
		if (killit) {
			/* Try giving it some time to disconnect cleanly */
			if (kill (priv->pid, SIGTERM) == 0)
				g_timeout_add (2000, ensure_killed, GINT_TO_POINTER (priv->pid));
			_LOGI ("Terminated vpnc daemon with PID %d.", priv->pid);
		} else {
			/* Already quit, just reap the child */
			waitpid (priv->pid, NULL, WNOHANG);
		}
		priv->pid = 0;
	}
}