seamless_init(void)
{
	if (!g_seamless_rdp)
		return False;

	seamless_serial = 0;

	seamless_channel =
		channel_register("seamrdp", CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
				 seamless_process);
	return (seamless_channel != NULL);
}