seamless_send_sync()
{
	if (!g_seamless_rdp)
		return (unsigned int) -1;

	return seamless_send("SYNC", "");
}