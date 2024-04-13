int aarp_send_ddp(struct net_device *dev, struct sk_buff *skb,
		  struct atalk_addr *sa, void *hwaddr)
{
	static char ddp_eth_multicast[ETH_ALEN] =
		{ 0x09, 0x00, 0x07, 0xFF, 0xFF, 0xFF };
	int hash;
	struct aarp_entry *a;

	skb_reset_network_header(skb);

	/* Check for LocalTalk first */
	if (dev->type == ARPHRD_LOCALTLK) {
		struct atalk_addr *at = atalk_find_dev_addr(dev);
		struct ddpehdr *ddp = (struct ddpehdr *)skb->data;
		int ft = 2;

		/*
		 * Compressible ?
		 *
		 * IFF: src_net == dest_net == device_net
		 * (zero matches anything)
		 */

		if ((!ddp->deh_snet || at->s_net == ddp->deh_snet) &&
		    (!ddp->deh_dnet || at->s_net == ddp->deh_dnet)) {
			skb_pull(skb, sizeof(*ddp) - 4);

			/*
			 *	The upper two remaining bytes are the port
			 *	numbers	we just happen to need. Now put the
			 *	length in the lower two.
			 */
			*((__be16 *)skb->data) = htons(skb->len);
			ft = 1;
		}
		/*
		 * Nice and easy. No AARP type protocols occur here so we can
		 * just shovel it out with a 3 byte LLAP header
		 */

		skb_push(skb, 3);
		skb->data[0] = sa->s_node;
		skb->data[1] = at->s_node;
		skb->data[2] = ft;
		skb->dev     = dev;
		goto sendit;
	}

	/* On a PPP link we neither compress nor aarp.  */
	if (dev->type == ARPHRD_PPP) {
		skb->protocol = htons(ETH_P_PPPTALK);
		skb->dev = dev;
		goto sendit;
	}

	/* Non ELAP we cannot do. */
	if (dev->type != ARPHRD_ETHER)
		goto free_it;

	skb->dev = dev;
	skb->protocol = htons(ETH_P_ATALK);
	hash = sa->s_node % (AARP_HASH_SIZE - 1);

	/* Do we have a resolved entry? */
	if (sa->s_node == ATADDR_BCAST) {
		/* Send it */
		ddp_dl->request(ddp_dl, skb, ddp_eth_multicast);
		goto sent;
	}

	write_lock_bh(&aarp_lock);
	a = __aarp_find_entry(resolved[hash], dev, sa);

	if (a) { /* Return 1 and fill in the address */
		a->expires_at = jiffies + (sysctl_aarp_expiry_time * 10);
		ddp_dl->request(ddp_dl, skb, a->hwaddr);
		write_unlock_bh(&aarp_lock);
		goto sent;
	}

	/* Do we have an unresolved entry: This is the less common path */
	a = __aarp_find_entry(unresolved[hash], dev, sa);
	if (a) { /* Queue onto the unresolved queue */
		skb_queue_tail(&a->packet_queue, skb);
		goto out_unlock;
	}

	/* Allocate a new entry */
	a = aarp_alloc();
	if (!a) {
		/* Whoops slipped... good job it's an unreliable protocol 8) */
		write_unlock_bh(&aarp_lock);
		goto free_it;
	}

	/* Set up the queue */
	skb_queue_tail(&a->packet_queue, skb);
	a->expires_at	 = jiffies + sysctl_aarp_resolve_time;
	a->dev		 = dev;
	a->next		 = unresolved[hash];
	a->target_addr	 = *sa;
	a->xmit_count	 = 0;
	unresolved[hash] = a;
	unresolved_count++;

	/* Send an initial request for the address */
	__aarp_send_query(a);

	/*
	 * Switch to fast timer if needed (That is if this is the first
	 * unresolved entry to get added)
	 */

	if (unresolved_count == 1)
		mod_timer(&aarp_timer, jiffies + sysctl_aarp_tick_time);

	/* Now finally, it is safe to drop the lock. */
out_unlock:
	write_unlock_bh(&aarp_lock);

	/* Tell the ddp layer we have taken over for this frame. */
	goto sent;

sendit:
	if (skb->sk)
		skb->priority = skb->sk->sk_priority;
	if (dev_queue_xmit(skb))
		goto drop;
sent:
	return NET_XMIT_SUCCESS;
free_it:
	kfree_skb(skb);
drop:
	return NET_XMIT_DROP;
}