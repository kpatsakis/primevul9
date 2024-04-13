remove_pidfile (NMVPNCPlugin *plugin)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);

	if (priv->pid_file) {
		unlink (priv->pid_file);
		g_free (priv->pid_file);
		priv->pid_file = NULL;
	}
}