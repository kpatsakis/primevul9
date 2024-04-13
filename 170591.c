int aarp_proxy_probe_network(struct atalk_iface *atif, struct atalk_addr *sa)
{
	int hash, retval = -EPROTONOSUPPORT;
	struct aarp_entry *entry;
	unsigned int count;

	/*
	 * we don't currently support LocalTalk or PPP for proxy AARP;
	 * if someone wants to try and add it, have fun
	 */
	if (atif->dev->type == ARPHRD_LOCALTLK ||
	    atif->dev->type == ARPHRD_PPP)
		goto out;

	/*
	 * create a new AARP entry with the flags set to be published --
	 * we need this one to hang around even if it's in use
	 */
	entry = aarp_alloc();
	retval = -ENOMEM;
	if (!entry)
		goto out;

	entry->expires_at = -1;
	entry->status = ATIF_PROBE;
	entry->target_addr.s_node = sa->s_node;
	entry->target_addr.s_net = sa->s_net;
	entry->dev = atif->dev;

	write_lock_bh(&aarp_lock);

	hash = sa->s_node % (AARP_HASH_SIZE - 1);
	entry->next = proxies[hash];
	proxies[hash] = entry;

	for (count = 0; count < AARP_RETRANSMIT_LIMIT; count++) {
		aarp_send_probe(atif->dev, sa);

		/* Defer 1/10th */
		write_unlock_bh(&aarp_lock);
		msleep(100);
		write_lock_bh(&aarp_lock);

		if (entry->status & ATIF_PROBE_FAIL)
			break;
	}

	if (entry->status & ATIF_PROBE_FAIL) {
		entry->expires_at = jiffies - 1; /* free the entry */
		retval = -EADDRINUSE; /* return network full */
	} else { /* clear the probing flag */
		entry->status &= ~ATIF_PROBE;
		retval = 1;
	}

	write_unlock_bh(&aarp_lock);
out:
	return retval;
}