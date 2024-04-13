isdn_net_ciscohdlck_receive(isdn_net_local *lp, struct sk_buff *skb)
{
	unsigned char *p;
	u8 addr;
	u8 ctrl;
	u16 type;

	if (skb->len < 4)
		goto out_free;

	p = skb->data;
	addr = *(u8 *)(p + 0);
	ctrl = *(u8 *)(p + 1);
	type = be16_to_cpup((__be16 *)(p + 2));
	p += 4;
	skb_pull(skb, 4);

	if (addr != CISCO_ADDR_UNICAST && addr != CISCO_ADDR_BROADCAST) {
		printk(KERN_WARNING "%s: Unknown Cisco addr 0x%02x\n",
		       lp->netdev->dev->name, addr);
		goto out_free;
	}
	if (ctrl != CISCO_CTRL) {
		printk(KERN_WARNING "%s: Unknown Cisco ctrl 0x%02x\n",
		       lp->netdev->dev->name, ctrl);
		goto out_free;
	}

	switch (type) {
	case CISCO_TYPE_SLARP:
		isdn_net_ciscohdlck_slarp_in(lp, skb);
		goto out_free;
	case CISCO_TYPE_CDP:
		if (lp->cisco_debserint)
			printk(KERN_DEBUG "%s: Received CDP packet. use "
			       "\"no cdp enable\" on cisco.\n",
			       lp->netdev->dev->name);
		goto out_free;
	default:
		/* no special cisco protocol */
		skb->protocol = htons(type);
		netif_rx(skb);
		return;
	}

out_free:
	kfree_skb(skb);
}