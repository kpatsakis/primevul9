static DWORD WINAPI shadow_server_thread(LPVOID arg)
{
	rdpShadowServer* server = (rdpShadowServer*)arg;
	BOOL running = TRUE;
	DWORD status;
	freerdp_listener* listener = server->listener;
	shadow_subsystem_start(server->subsystem);

	while (running)
	{
		HANDLE events[32];
		DWORD nCount = 0;
		events[nCount++] = server->StopEvent;
		nCount += listener->GetEventHandles(listener, &events[nCount], 32 - nCount);

		if (nCount <= 1)
		{
			WLog_ERR(TAG, "Failed to get FreeRDP file descriptor");
			break;
		}

		status = WaitForMultipleObjects(nCount, events, FALSE, INFINITE);

		switch (status)
		{
			case WAIT_FAILED:
			case WAIT_OBJECT_0:
				running = FALSE;
				break;

			default:
			{
				if (!listener->CheckFileDescriptor(listener))
				{
					WLog_ERR(TAG, "Failed to check FreeRDP file descriptor");
					running = FALSE;
				}
				else
				{
#ifdef _WIN32
					Sleep(100); /* FIXME: listener event handles */
#endif
				}
			}
			break;
		}
	}

	listener->Close(listener);
	shadow_subsystem_stop(server->subsystem);

	/* Signal to the clients that server is being stopped and wait for them
	 * to disconnect. */
	if (shadow_client_boardcast_quit(server, 0))
	{
		while (ArrayList_Count(server->clients) > 0)
		{
			Sleep(100);
		}
	}

	ExitThread(0);
	return 0;
}