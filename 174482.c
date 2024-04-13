rdpdr_init()
{
	rdpdr_channel =
		channel_register("rdpdr",
				 CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_COMPRESS_RDP,
				 rdpdr_process);

	return (rdpdr_channel != NULL);
}