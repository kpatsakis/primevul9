nm_vpnc_plugin_init (NMVPNCPlugin *plugin)
{
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);

	priv->infd = -1;
	priv->out.fd = -1;
	priv->err.fd = -1;
	priv->server_message = g_string_sized_new (30);
}