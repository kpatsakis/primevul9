static int check_port_perm(struct snd_seq_client_port *port, unsigned int flags)
{
	if ((port->capability & flags) != flags)
		return 0;
	return flags;
}