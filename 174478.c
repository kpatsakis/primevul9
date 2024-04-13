seamless_send_persistent(RD_BOOL enable)
{
	unsigned int res;
	if (!g_seamless_rdp)
		return (unsigned int) -1;
	printf("%s persistent seamless mode.\n", enable?"Enable":"Disable");
	res = seamless_send("PERSISTENT", "%d", enable);
	
	return res;
}