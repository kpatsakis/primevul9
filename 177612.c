isdn_net_ciscohdlck_alloc_skb(isdn_net_local *lp, int len)
{
	unsigned short hl = dev->drv[lp->isdn_device]->interface->hl_hdrlen;
	struct sk_buff *skb;

	skb = alloc_skb(hl + len, GFP_ATOMIC);
	if (skb)
		skb_reserve(skb, hl);
	else
		printk("isdn out of mem at %s:%d!\n", __FILE__, __LINE__);
	return skb;
}