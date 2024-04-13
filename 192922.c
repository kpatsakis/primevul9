int snd_seq_kernel_client_ctl(int clientid, unsigned int cmd, void *arg)
{
	const struct ioctl_handler *handler;
	struct snd_seq_client *client;

	client = clientptr(clientid);
	if (client == NULL)
		return -ENXIO;

	for (handler = ioctl_handlers; handler->cmd > 0; ++handler) {
		if (handler->cmd == cmd)
			return handler->func(client, arg);
	}

	pr_debug("ALSA: seq unknown ioctl() 0x%x (type='%c', number=0x%02x)\n",
		 cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));
	return -ENOTTY;
}