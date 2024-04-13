int shadow_server_stop(rdpShadowServer* server)
{
	if (!server)
		return -1;

	if (server->thread)
	{
		SetEvent(server->StopEvent);
		WaitForSingleObject(server->thread, INFINITE);
		CloseHandle(server->thread);
		server->thread = NULL;
		server->listener->Close(server->listener);
	}

	if (server->screen)
	{
		shadow_screen_free(server->screen);
		server->screen = NULL;
	}

	if (server->capture)
	{
		shadow_capture_free(server->capture);
		server->capture = NULL;
	}

	return 0;
}