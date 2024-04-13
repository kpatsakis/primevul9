isdn_net_ciscohdlck_slarp_send_keepalive(unsigned long data)
{
	isdn_net_local *lp = (isdn_net_local *) data;
	struct sk_buff *skb;
	unsigned char *p;
	unsigned long last_cisco_myseq = lp->cisco_myseq;
	int myseq_diff = 0;

	if (!(lp->flags & ISDN_NET_CONNECTED) || lp->dialstate) {
		printk("isdn BUG at %s:%d!\n", __FILE__, __LINE__);
		return;
	}
	lp->cisco_myseq++;

	myseq_diff = (lp->cisco_myseq - lp->cisco_mineseen);
	if ((lp->cisco_line_state) && ((myseq_diff >= 3) || (myseq_diff <= -3))) {
		/* line up -> down */
		lp->cisco_line_state = 0;
		printk(KERN_WARNING
		       "UPDOWN: Line protocol on Interface %s,"
		       " changed state to down\n", lp->netdev->dev->name);
		/* should stop routing higher-level data across */
	} else if ((!lp->cisco_line_state) &&
		   (myseq_diff >= 0) && (myseq_diff <= 2)) {
		/* line down -> up */
		lp->cisco_line_state = 1;
		printk(KERN_WARNING
		       "UPDOWN: Line protocol on Interface %s,"
		       " changed state to up\n", lp->netdev->dev->name);
		/* restart routing higher-level data across */
	}

	if (lp->cisco_debserint)
		printk(KERN_DEBUG "%s: HDLC "
		       "myseq %lu, mineseen %lu%c, yourseen %lu, %s\n",
		       lp->netdev->dev->name, last_cisco_myseq, lp->cisco_mineseen,
		       ((last_cisco_myseq == lp->cisco_mineseen) ? '*' : 040),
		       lp->cisco_yourseq,
		       ((lp->cisco_line_state) ? "line up" : "line down"));

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	*(u8 *)(p + 0) = CISCO_ADDR_UNICAST;
	*(u8 *)(p + 1) = CISCO_CTRL;
	*(__be16 *)(p + 2) = cpu_to_be16(CISCO_TYPE_SLARP);

	/* slarp keepalive */
	*(__be32 *)(p +  4) = cpu_to_be32(CISCO_SLARP_KEEPALIVE);
	*(__be32 *)(p +  8) = cpu_to_be32(lp->cisco_myseq);
	*(__be32 *)(p + 12) = cpu_to_be32(lp->cisco_yourseq);
	*(__be16 *)(p + 16) = cpu_to_be16(0xffff); // reliability, always 0xffff
	p += 18;

	isdn_net_write_super(lp, skb);

	lp->cisco_timer.expires = jiffies + lp->cisco_keepalive_period * HZ;

	add_timer(&lp->cisco_timer);
}