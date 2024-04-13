static int aarp_rcv(struct sk_buff *skb, struct net_device *dev,
		    struct packet_type *pt, struct net_device *orig_dev)
{
	struct elapaarp *ea = aarp_hdr(skb);
	int hash, ret = 0;
	__u16 function;
	struct aarp_entry *a;
	struct atalk_addr sa, *ma, da;
	struct atalk_iface *ifa;

	if (!net_eq(dev_net(dev), &init_net))
		goto out0;

	/* We only do Ethernet SNAP AARP. */
	if (dev->type != ARPHRD_ETHER)
		goto out0;

	/* Frame size ok? */
	if (!skb_pull(skb, sizeof(*ea)))
		goto out0;

	function = ntohs(ea->function);

	/* Sanity check fields. */
	if (function < AARP_REQUEST || function > AARP_PROBE ||
	    ea->hw_len != ETH_ALEN || ea->pa_len != AARP_PA_ALEN ||
	    ea->pa_src_zero || ea->pa_dst_zero)
		goto out0;

	/* Looks good. */
	hash = ea->pa_src_node % (AARP_HASH_SIZE - 1);

	/* Build an address. */
	sa.s_node = ea->pa_src_node;
	sa.s_net = ea->pa_src_net;

	/* Process the packet. Check for replies of me. */
	ifa = atalk_find_dev(dev);
	if (!ifa)
		goto out1;

	if (ifa->status & ATIF_PROBE &&
	    ifa->address.s_node == ea->pa_dst_node &&
	    ifa->address.s_net == ea->pa_dst_net) {
		ifa->status |= ATIF_PROBE_FAIL; /* Fail the probe (in use) */
		goto out1;
	}

	/* Check for replies of proxy AARP entries */
	da.s_node = ea->pa_dst_node;
	da.s_net  = ea->pa_dst_net;

	write_lock_bh(&aarp_lock);
	a = __aarp_find_entry(proxies[hash], dev, &da);

	if (a && a->status & ATIF_PROBE) {
		a->status |= ATIF_PROBE_FAIL;
		/*
		 * we do not respond to probe or request packets for
		 * this address while we are probing this address
		 */
		goto unlock;
	}

	switch (function) {
	case AARP_REPLY:
		if (!unresolved_count)	/* Speed up */
			break;

		/* Find the entry.  */
		a = __aarp_find_entry(unresolved[hash], dev, &sa);
		if (!a || dev != a->dev)
			break;

		/* We can fill one in - this is good. */
		ether_addr_copy(a->hwaddr, ea->hw_src);
		__aarp_resolved(&unresolved[hash], a, hash);
		if (!unresolved_count)
			mod_timer(&aarp_timer,
				  jiffies + sysctl_aarp_expiry_time);
		break;

	case AARP_REQUEST:
	case AARP_PROBE:

		/*
		 * If it is my address set ma to my address and reply.
		 * We can treat probe and request the same.  Probe
		 * simply means we shouldn't cache the querying host,
		 * as in a probe they are proposing an address not
		 * using one.
		 *
		 * Support for proxy-AARP added. We check if the
		 * address is one of our proxies before we toss the
		 * packet out.
		 */

		sa.s_node = ea->pa_dst_node;
		sa.s_net  = ea->pa_dst_net;

		/* See if we have a matching proxy. */
		ma = __aarp_proxy_find(dev, &sa);
		if (!ma)
			ma = &ifa->address;
		else { /* We need to make a copy of the entry. */
			da.s_node = sa.s_node;
			da.s_net = sa.s_net;
			ma = &da;
		}

		if (function == AARP_PROBE) {
			/*
			 * A probe implies someone trying to get an
			 * address. So as a precaution flush any
			 * entries we have for this address.
			 */
			a = __aarp_find_entry(resolved[sa.s_node %
						       (AARP_HASH_SIZE - 1)],
					      skb->dev, &sa);

			/*
			 * Make it expire next tick - that avoids us
			 * getting into a probe/flush/learn/probe/
			 * flush/learn cycle during probing of a slow
			 * to respond host addr.
			 */
			if (a) {
				a->expires_at = jiffies - 1;
				mod_timer(&aarp_timer, jiffies +
					  sysctl_aarp_tick_time);
			}
		}

		if (sa.s_node != ma->s_node)
			break;

		if (sa.s_net && ma->s_net && sa.s_net != ma->s_net)
			break;

		sa.s_node = ea->pa_src_node;
		sa.s_net = ea->pa_src_net;

		/* aarp_my_address has found the address to use for us.
		 */
		aarp_send_reply(dev, ma, &sa, ea->hw_src);
		break;
	}

unlock:
	write_unlock_bh(&aarp_lock);
out1:
	ret = 1;
out0:
	kfree_skb(skb);
	return ret;
}