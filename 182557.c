real_disconnect (NMVpnServicePlugin *plugin, GError **error)
{
	vpnc_cleanup (NM_VPNC_PLUGIN (plugin), TRUE);
	return TRUE;
}