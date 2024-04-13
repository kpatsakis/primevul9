rdp_connect(char *server, uint32 flags, char *domain, char *password,
	    char *command, char *directory, RD_BOOL reconnect)
{
	RD_BOOL deactivated = False;
	uint32 ext_disc_reason = 0;

	if (!sec_connect(server, g_username, domain, password, reconnect))
		return False;

	rdp_send_logon_info(flags, domain, g_username, password, command, directory);

	/* run RDP loop until first licence demand active PDU */
	while (!g_rdp_shareid)
	{
		if (g_network_error)
			return False;

		if (!rdp_loop(&deactivated, &ext_disc_reason))
			return False;

		if (g_redirect)
			return True;
	}
	return True;
}