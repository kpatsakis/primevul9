real_connect (NMVpnServicePlugin   *plugin,
              NMConnection  *connection,
              GError       **error)
{
	return _connect_common (plugin, FALSE, connection, NULL, error);
}