static int snd_seq_ioctl_unsubscribe_port(struct snd_seq_client *client,
					  void *arg)
{
	struct snd_seq_port_subscribe *subs = arg;
	int result = -ENXIO;
	struct snd_seq_client *receiver = NULL, *sender = NULL;
	struct snd_seq_client_port *sport = NULL, *dport = NULL;

	if ((receiver = snd_seq_client_use_ptr(subs->dest.client)) == NULL)
		goto __end;
	if ((sender = snd_seq_client_use_ptr(subs->sender.client)) == NULL)
		goto __end;
	if ((sport = snd_seq_port_use_ptr(sender, subs->sender.port)) == NULL)
		goto __end;
	if ((dport = snd_seq_port_use_ptr(receiver, subs->dest.port)) == NULL)
		goto __end;

	result = check_subscription_permission(client, sport, dport, subs);
	if (result < 0)
		goto __end;

	result = snd_seq_port_disconnect(client, sender, sport, receiver, dport, subs);
	if (! result) /* broadcast announce */
		snd_seq_client_notify_subscription(SNDRV_SEQ_ADDRESS_SUBSCRIBERS, 0,
						   subs, SNDRV_SEQ_EVENT_PORT_UNSUBSCRIBED);
      __end:
      	if (sport)
		snd_seq_port_unlock(sport);
	if (dport)
		snd_seq_port_unlock(dport);
	if (sender)
		snd_seq_client_unlock(sender);
	if (receiver)
		snd_seq_client_unlock(receiver);
	return result;
}