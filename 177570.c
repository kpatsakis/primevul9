isdn_net_ciscohdlck_slarp_in(isdn_net_local *lp, struct sk_buff *skb)
{
	unsigned char *p;
	int period;
	u32 code;
	u32 my_seq;
	u32 your_seq;
	__be32 local;
	__be32 *addr, *mask;

	if (skb->len < 14)
		return;

	p = skb->data;
	code = be32_to_cpup((__be32 *)p);
	p += 4;

	switch (code) {
	case CISCO_SLARP_REQUEST:
		lp->cisco_yourseq = 0;
		isdn_net_ciscohdlck_slarp_send_reply(lp);
		break;
	case CISCO_SLARP_REPLY:
		addr = (__be32 *)p;
		mask = (__be32 *)(p + 4);
		if (*mask != cpu_to_be32(0xfffffffc))
			goto slarp_reply_out;
		if ((*addr & cpu_to_be32(3)) == cpu_to_be32(0) ||
		    (*addr & cpu_to_be32(3)) == cpu_to_be32(3))
			goto slarp_reply_out;
		local = *addr ^ cpu_to_be32(3);
		printk(KERN_INFO "%s: got slarp reply: remote ip: %pI4, local ip: %pI4 mask: %pI4\n",
		       lp->netdev->dev->name, addr, &local, mask);
		break;
	slarp_reply_out:
		printk(KERN_INFO "%s: got invalid slarp reply (%pI4/%pI4) - ignored\n",
		       lp->netdev->dev->name, addr, mask);
		break;
	case CISCO_SLARP_KEEPALIVE:
		period = (int)((jiffies - lp->cisco_last_slarp_in
				+ HZ / 2 - 1) / HZ);
		if (lp->cisco_debserint &&
		    (period != lp->cisco_keepalive_period) &&
		    lp->cisco_last_slarp_in) {
			printk(KERN_DEBUG "%s: Keepalive period mismatch - "
			       "is %d but should be %d.\n",
			       lp->netdev->dev->name, period,
			       lp->cisco_keepalive_period);
		}
		lp->cisco_last_slarp_in = jiffies;
		my_seq = be32_to_cpup((__be32 *)(p + 0));
		your_seq = be32_to_cpup((__be32 *)(p + 4));
		p += 10;
		lp->cisco_yourseq = my_seq;
		lp->cisco_mineseen = your_seq;
		break;
	}
}