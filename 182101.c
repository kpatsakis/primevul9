static void process_capabilities(const char *line, int *len)
{
	int nul_location = strlen(line);
	if (nul_location == *len)
		return;
	server_capabilities_v1 = xstrdup(line + nul_location + 1);
	*len = nul_location;
}