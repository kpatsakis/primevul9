int shadow_server_init(rdpShadowServer* server)
{
	int status;
	winpr_InitializeSSL(WINPR_SSL_INIT_DEFAULT);
	WTSRegisterWtsApiFunctionTable(FreeRDP_InitWtsApi());

	if (!(server->clients = ArrayList_New(TRUE)))
		goto fail_client_array;

	if (!(server->StopEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		goto fail_stop_event;

	if (!InitializeCriticalSectionAndSpinCount(&(server->lock), 4000))
		goto fail_server_lock;

	status = shadow_server_init_config_path(server);

	if (status < 0)
		goto fail_config_path;

	status = shadow_server_init_certificate(server);

	if (status < 0)
		goto fail_certificate;

	server->listener = freerdp_listener_new();

	if (!server->listener)
		goto fail_listener;

	server->listener->info = (void*)server;
	server->listener->PeerAccepted = shadow_client_accepted;
	server->subsystem = shadow_subsystem_new();

	if (!server->subsystem)
		goto fail_subsystem_new;

	status = shadow_subsystem_init(server->subsystem, server);

	if (status >= 0)
		return status;

	shadow_subsystem_free(server->subsystem);
fail_subsystem_new:
	freerdp_listener_free(server->listener);
	server->listener = NULL;
fail_listener:
	free(server->CertificateFile);
	server->CertificateFile = NULL;
	free(server->PrivateKeyFile);
	server->PrivateKeyFile = NULL;
fail_certificate:
	free(server->ConfigPath);
	server->ConfigPath = NULL;
fail_config_path:
	DeleteCriticalSection(&(server->lock));
fail_server_lock:
	CloseHandle(server->StopEvent);
	server->StopEvent = NULL;
fail_stop_event:
	ArrayList_Free(server->clients);
	server->clients = NULL;
fail_client_array:
	WLog_ERR(TAG, "Failed to initialize shadow server");
	return -1;
}