lspci_init(void)
{
	lspci_channel =
		channel_register("lspci", CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
				 lspci_process);
	return (lspci_channel != NULL);
}