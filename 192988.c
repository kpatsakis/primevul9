static int check_subscription_permission(struct snd_seq_client *client,
					 struct snd_seq_client_port *sport,
					 struct snd_seq_client_port *dport,
					 struct snd_seq_port_subscribe *subs)
{
	if (client->number != subs->sender.client &&
	    client->number != subs->dest.client) {
		/* connection by third client - check export permission */
		if (check_port_perm(sport, SNDRV_SEQ_PORT_CAP_NO_EXPORT))
			return -EPERM;
		if (check_port_perm(dport, SNDRV_SEQ_PORT_CAP_NO_EXPORT))
			return -EPERM;
	}

	/* check read permission */
	/* if sender or receiver is the subscribing client itself,
	 * no permission check is necessary
	 */
	if (client->number != subs->sender.client) {
		if (! check_port_perm(sport, PERM_RD))
			return -EPERM;
	}
	/* check write permission */
	if (client->number != subs->dest.client) {
		if (! check_port_perm(dport, PERM_WR))
			return -EPERM;
	}
	return 0;
}