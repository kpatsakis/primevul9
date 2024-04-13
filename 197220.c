static BOOL open_port(rdpShadowServer* server, char* address)
{
	BOOL status;
	char* modaddr = address;

	if (modaddr)
	{
		if (modaddr[0] == '[')
		{
			char* end = strchr(address, ']');
			if (!end)
			{
				WLog_ERR(TAG, "Could not parse bind-address %s", address);
				return -1;
			}
			*end++ = '\0';
			if (strlen(end) > 0)
			{
				WLog_ERR(TAG, "Excess data after IPv6 address: '%s'", end);
				return -1;
			}
			modaddr++;
		}
	}
	status = server->listener->Open(server->listener, modaddr, (UINT16)server->port);

	if (!status)
	{
		WLog_ERR(TAG,
		         "Problem creating TCP listener. (Port already used or insufficient permissions?)");
	}

	return status;
}