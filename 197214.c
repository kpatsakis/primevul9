int shadow_server_uninit(rdpShadowServer* server)
{
	if (!server)
		return -1;

	shadow_server_stop(server);
	shadow_subsystem_uninit(server->subsystem);
	shadow_subsystem_free(server->subsystem);
	freerdp_listener_free(server->listener);
	server->listener = NULL;
	free(server->CertificateFile);
	server->CertificateFile = NULL;
	free(server->PrivateKeyFile);
	server->PrivateKeyFile = NULL;
	free(server->ConfigPath);
	server->ConfigPath = NULL;
	DeleteCriticalSection(&(server->lock));
	CloseHandle(server->StopEvent);
	server->StopEvent = NULL;
	ArrayList_Free(server->clients);
	server->clients = NULL;
	return 1;
}