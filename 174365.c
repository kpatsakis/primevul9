seamless_send_spawn(char *cmdline)
{
	unsigned int res;
	if (!g_seamless_rdp)
		return (unsigned int) -1;

	res = seamless_send("SPAWN", cmdline);

	return res;
}