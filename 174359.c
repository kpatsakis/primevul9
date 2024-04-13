cliprdr_init(void)
{
	cliprdr_channel =
		channel_register("cliprdr",
				 CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP |
				 CHANNEL_OPTION_COMPRESS_RDP | CHANNEL_OPTION_SHOW_PROTOCOL,
				 cliprdr_process);
	return (cliprdr_channel != NULL);
}