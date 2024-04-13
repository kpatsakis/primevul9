void shadow_server_free(rdpShadowServer* server)
{
	if (!server)
		return;

	free(server->ipcSocket);
	server->ipcSocket = NULL;
	freerdp_settings_free(server->settings);
	server->settings = NULL;
	free(server);
}