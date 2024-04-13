mcs_connect_start(char *server, char *username, char *domain, char *password,
		  RD_BOOL reconnect, uint32 * selected_protocol)
{
	return iso_connect(server, username, domain, password, reconnect, selected_protocol);
}