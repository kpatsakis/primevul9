int snd_seq_client_notify_subscription(int client, int port,
				       struct snd_seq_port_subscribe *info,
				       int evtype)
{
	struct snd_seq_event event;

	memset(&event, 0, sizeof(event));
	event.type = evtype;
	event.data.connect.dest = info->dest;
	event.data.connect.sender = info->sender;

	return snd_seq_system_notify(client, port, &event);  /* non-atomic */
}