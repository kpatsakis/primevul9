int bond_arp_rcv(const struct sk_buff *skb, struct bonding *bond,
		 struct slave *slave)
{
	struct arphdr *arp = (struct arphdr *)skb->data;
	struct slave *curr_active_slave, *curr_arp_slave;
	unsigned char *arp_ptr;
	__be32 sip, tip;
	int is_arp = skb->protocol == __cpu_to_be16(ETH_P_ARP);
	unsigned int alen;

	if (!slave_do_arp_validate(bond, slave)) {
		if ((slave_do_arp_validate_only(bond) && is_arp) ||
		    !slave_do_arp_validate_only(bond))
			slave->last_rx = jiffies;
		return RX_HANDLER_ANOTHER;
	} else if (!is_arp) {
		return RX_HANDLER_ANOTHER;
	}

	alen = arp_hdr_len(bond->dev);

	slave_dbg(bond->dev, slave->dev, "%s: skb->dev %s\n",
		   __func__, skb->dev->name);

	if (alen > skb_headlen(skb)) {
		arp = kmalloc(alen, GFP_ATOMIC);
		if (!arp)
			goto out_unlock;
		if (skb_copy_bits(skb, 0, arp, alen) < 0)
			goto out_unlock;
	}

	if (arp->ar_hln != bond->dev->addr_len ||
	    skb->pkt_type == PACKET_OTHERHOST ||
	    skb->pkt_type == PACKET_LOOPBACK ||
	    arp->ar_hrd != htons(ARPHRD_ETHER) ||
	    arp->ar_pro != htons(ETH_P_IP) ||
	    arp->ar_pln != 4)
		goto out_unlock;

	arp_ptr = (unsigned char *)(arp + 1);
	arp_ptr += bond->dev->addr_len;
	memcpy(&sip, arp_ptr, 4);
	arp_ptr += 4 + bond->dev->addr_len;
	memcpy(&tip, arp_ptr, 4);

	slave_dbg(bond->dev, slave->dev, "%s: %s/%d av %d sv %d sip %pI4 tip %pI4\n",
		  __func__, slave->dev->name, bond_slave_state(slave),
		  bond->params.arp_validate, slave_do_arp_validate(bond, slave),
		  &sip, &tip);

	curr_active_slave = rcu_dereference(bond->curr_active_slave);
	curr_arp_slave = rcu_dereference(bond->current_arp_slave);

	/* We 'trust' the received ARP enough to validate it if:
	 *
	 * (a) the slave receiving the ARP is active (which includes the
	 * current ARP slave, if any), or
	 *
	 * (b) the receiving slave isn't active, but there is a currently
	 * active slave and it received valid arp reply(s) after it became
	 * the currently active slave, or
	 *
	 * (c) there is an ARP slave that sent an ARP during the prior ARP
	 * interval, and we receive an ARP reply on any slave.  We accept
	 * these because switch FDB update delays may deliver the ARP
	 * reply to a slave other than the sender of the ARP request.
	 *
	 * Note: for (b), backup slaves are receiving the broadcast ARP
	 * request, not a reply.  This request passes from the sending
	 * slave through the L2 switch(es) to the receiving slave.  Since
	 * this is checking the request, sip/tip are swapped for
	 * validation.
	 *
	 * This is done to avoid endless looping when we can't reach the
	 * arp_ip_target and fool ourselves with our own arp requests.
	 */
	if (bond_is_active_slave(slave))
		bond_validate_arp(bond, slave, sip, tip);
	else if (curr_active_slave &&
		 time_after(slave_last_rx(bond, curr_active_slave),
			    curr_active_slave->last_link_up))
		bond_validate_arp(bond, slave, tip, sip);
	else if (curr_arp_slave && (arp->ar_op == htons(ARPOP_REPLY)) &&
		 bond_time_in_interval(bond,
				       dev_trans_start(curr_arp_slave->dev), 1))
		bond_validate_arp(bond, slave, sip, tip);

out_unlock:
	if (arp != (struct arphdr *)skb->data)
		kfree(arp);
	return RX_HANDLER_ANOTHER;
}